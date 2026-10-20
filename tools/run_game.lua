-- PCSX-Redux script for tools/run_game.py: plays a schedule of pad presses frame by frame and records each frame.
--
-- RUN_GAME_SCHEDULE names the schedule, one command per line:
--   clock game END_FRAME HANDLER TIMER TICKS
--                             count frames in game frames, not vsyncs, and pin the game's time to them (below);
--                             END_FRAME, HANDLER and TIMER are the addresses of Gfx_EndFrame, Sys_TimerHandler and
--                             g_TimerTicks in the executable run, in hexadecimal
--   limit VSYNCS              with the game clock, quit at this vsync whatever the game frame
--   pace VSYNC FIRST LAST COUNT
--                             with the game clock, make game frames FIRST to LAST last at least COUNT vsyncs each:
--                             VSYNC is the address of the SDK's VSync, in hexadecimal, whose first call with 0
--                             after Gfx_EndFrame's entry is given COUNT instead (wait until COUNT vsyncs have passed
--                             since its last return)
--   press FIRST LAST BUTTON   hold BUTTON (PCSX.CONSTS.PAD.BUTTON's name) from frame FIRST to frame LAST
--   shot FRAME                save the displayed image at FRAME
--   dump FRAME                with the game clock, save main RAM as game frame FRAME is displayed (ram_FRAME.bin)
--   end FRAME                 quit at FRAME
-- RUN_GAME_OUT names the directory written: trace.txt (one line a vsync: its number, a hash of the displayed image,
-- the program counter, the last exception's code), shot_FRAME_WIDTHxHEIGHT_BPP.raw for each shot, the displayed image
-- as the emulator gives it, and with the game clock game_frames.txt (one line a game frame: its number, the vsync it
-- was shown at, a hash of its image).
--
-- The game clock: the game measures a frame's length with g_TimerTicks, which Sys_TimerHandler counts up from a root
-- counter's interrupt, and moves its objects by it, so code of another speed plays another game. Each of the handler's
-- increments is undone before it happens, and on entering Gfx_EndFrame, which ends a game frame, g_TimerTicks is set to
-- the frame's number times TICKS: every frame lasts TICKS for the game however long it took. Presses then apply to the
-- game frame after the one ending, and the image recorded for a game frame is the one shown when the next one ends.
-- Exec breakpoints need the emulator's interpreter and debugger (-interpreter -debugger).

local FNV_OFFSET = 0xcbf29ce484222325ULL
local FNV_PRIME = 0x100000001b3ULL
local BYTES_PER_WORD = 4
local HEXADECIMAL = 16
-- Coprocessor 0's Cause register, and its ExcCode field: the kind of the last exception taken
local CP0_CAUSE = 13
local CAUSE_CODE_SHIFT = 2
local CAUSE_CODE_MASK = 0x1f
-- The SPU's Speed setting: 1 is real time, 0 or less unbounded
local UNBOUNDED_SPEED = 0
-- The emulated pads' input source: on Auto they read the host's gamepads, whose state reaches the game at moments that
-- vary from run to run; a headless emulator gets no keyboard events, so only the schedule's overrides press anything
local HOST_INPUT_NONE = 'Keyboard'
local PAD_PORTS = 2
-- Main RAM's size: an address's offset into it, whatever its segment (KUSEG, KSEG0, KSEG1)
local RAM_MASK = 0x1fffff
local RAM_SIZE = RAM_MASK + 1
local BREAKPOINT_WIDTH = 4

local schedulePath = os.getenv('RUN_GAME_SCHEDULE')
local outDir = os.getenv('RUN_GAME_OUT')
if schedulePath == nil or outDir == nil then
    error('RUN_GAME_SCHEDULE and RUN_GAME_OUT must be set')
end

local presses = {}
local shots = {}
local dumps = {}
local lastFrame = nil
local clock = nil
local vsyncLimit = nil
local pace = nil
for line in io.lines(schedulePath) do
    local words = {}
    for word in line:gmatch('%S+') do words[#words + 1] = word end
    if words[1] == 'press' then
        local button = PCSX.CONSTS.PAD.BUTTON[words[4]]
        if button == nil then error('unknown button ' .. words[4]) end
        presses[#presses + 1] = { first = tonumber(words[2]), last = tonumber(words[3]), button = button }
    elseif words[1] == 'shot' then
        shots[tonumber(words[2])] = true
    elseif words[1] == 'dump' then
        dumps[tonumber(words[2])] = true
    elseif words[1] == 'end' then
        lastFrame = tonumber(words[2])
    elseif words[1] == 'clock' and words[2] == 'game' then
        clock = { endFrame = tonumber(words[3], HEXADECIMAL), handler = tonumber(words[4], HEXADECIMAL),
                  timer = tonumber(words[5], HEXADECIMAL), ticks = tonumber(words[6]) }
        if clock.endFrame == nil or clock.handler == nil or clock.timer == nil or clock.ticks == nil then
            error('a clock line needs three hexadecimal addresses and a tick count')
        end
    elseif words[1] == 'limit' then
        vsyncLimit = tonumber(words[2])
    elseif words[1] == 'pace' then
        pace = { vsync = tonumber(words[2], HEXADECIMAL), first = tonumber(words[3]), last = tonumber(words[4]),
                 count = tonumber(words[5]) }
        if pace.vsync == nil or pace.first == nil or pace.last == nil or pace.count == nil then
            error('a pace line needs an address, two game frames and a vsync count')
        end
    elseif words[1] == nil then
        -- An empty line
    else
        error('unknown schedule command ' .. words[1])
    end
end
if lastFrame == nil then error('the schedule has no end') end
if clock ~= nil and vsyncLimit == nil then error('a schedule on the game clock needs a vsync limit') end
if pace ~= nil and clock == nil then error('pacing needs the game clock') end
if next(dumps) ~= nil and clock == nil then error('dumps count game frames: they need the game clock') end

-- The audio sink is the emulator's clock; unbounded, it lets the emulation run as fast as the host can
PCSX.settings.spu.Speed = UNBOUNDED_SPEED
for port = 1, PAD_PORTS do
    PCSX.settings.pads[port].PadType = HOST_INPUT_NONE
end

local trace = io.open(outDir .. '/trace.txt', 'w')
local gameFrames = nil
local pad = PCSX.SIO0.slots[1].pads[1]
local vsync = 0
local gameFrame = 0
-- Set on entering Gfx_EndFrame, cleared by the VSync call it paces
local pacePending = false

-- FNV-1a over the image's 32-bit words: enough to tell two images apart
local function hashImage(slice)
    local words = ffi.cast('const uint32_t *', slice.data)
    local hash = FNV_OFFSET
    for index = 0, slice.size / BYTES_PER_WORD - 1 do
        hash = bit.bxor(hash, words[index]) * FNV_PRIME
    end
    return hash
end

local function applyPresses(frame)
    for _, press in ipairs(presses) do
        if frame == press.first then
            pad.setOverride(press.button)
        elseif frame == press.last + 1 then
            pad.clearOverride(press.button)
        else
            -- Neither the press's start nor its end
        end
    end
end

local function saveShot(frame, shot)
    local name = string.format('%s/shot_%d_%dx%d_%d.raw', outDir, frame, shot.width, shot.height, tonumber(shot.bpp))
    local file = Support.File.open(name, 'TRUNCATE')
    file:write(shot.data)
    file:close()
end

local function finish()
    trace:close()
    if gameFrames ~= nil then gameFrames:close() end
    PCSX.quit(0)
end

if clock ~= nil then
    gameFrames = io.open(outDir .. '/game_frames.txt', 'w')
    local memory = PCSX.getMemPtr()
    local timer = ffi.cast('uint32_t *', memory + bit.band(clock.timer, RAM_MASK))

    -- Undone before the handler adds it: the root counter's time doesn't reach the game
    Crash2RunGameTimer = PCSX.addBreakpoint(clock.handler, 'Exec', BREAKPOINT_WIDTH, 'run_game timer', function()
        timer[0] = timer[0] - 1
        return true
    end)

    Crash2RunGameFrame = PCSX.addBreakpoint(clock.endFrame, 'Exec', BREAKPOINT_WIDTH, 'run_game frame', function()
        -- Entering the call that ends game frame gameFrame + 1, while game frame gameFrame is displayed
        local shot = PCSX.GPU.takeScreenShot()
        gameFrames:write(string.format('%d %d %s\n', gameFrame, vsync, bit.tohex(hashImage(shot.data), 16)))
        if shots[gameFrame] then
            saveShot(gameFrame, shot)
        end
        if dumps[gameFrame] then
            local file = io.open(string.format('%s/ram_%d.bin', outDir, gameFrame), 'wb')
            file:write(ffi.string(memory, RAM_SIZE))
            file:close()
        end
        gameFrame = gameFrame + 1
        timer[0] = gameFrame * clock.ticks
        pacePending = pace ~= nil and gameFrame >= pace.first and gameFrame <= pace.last
        applyPresses(gameFrame + 1)
        if gameFrame > lastFrame then
            finish()
        end
        return true
    end)

    if pace ~= nil then
        Crash2RunGamePace = PCSX.addBreakpoint(pace.vsync, 'Exec', BREAKPOINT_WIDTH, 'run_game pace', function()
            local registers = PCSX.getRegisters()
            if pacePending and registers.GPR.n.a0 == 0 then
                registers.GPR.n.a0 = pace.count
                pacePending = false
            else
                -- Another call: a count other than 0, or the frame already paced
            end
            return true
        end)
    else
        -- Each game frame as long as the code takes
    end
end

Crash2RunGameListener = PCSX.Events.createEventListener('GPU::Vsync', function()
    vsync = vsync + 1
    if clock == nil then
        applyPresses(vsync)
    end
    local shot = PCSX.GPU.takeScreenShot()
    local registers = PCSX.getRegisters()
    local exceptionCode = bit.band(bit.rshift(registers.CP0.r[CP0_CAUSE], CAUSE_CODE_SHIFT), CAUSE_CODE_MASK)
    trace:write(string.format('%d %s %08x %d\n', vsync, bit.tohex(hashImage(shot.data), 16), tonumber(registers.pc),
                              exceptionCode))
    if clock == nil and shots[vsync] then
        saveShot(vsync, shot)
    end
    if clock == nil and vsync >= lastFrame then
        finish()
    elseif clock ~= nil and vsync >= vsyncLimit then
        finish()
    else
        -- The run goes on
    end
end)
