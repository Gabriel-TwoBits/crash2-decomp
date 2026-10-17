-- PCSX-Redux script for tools/run_game.py: plays a schedule of pad presses frame by frame and records each frame.
--
-- RUN_GAME_SCHEDULE names the schedule, one command per line, frames counted in vsyncs from the start:
--   press FIRST LAST BUTTON   hold BUTTON (PCSX.CONSTS.PAD.BUTTON's name) from frame FIRST to frame LAST
--   shot FRAME                save the displayed image at FRAME
--   end FRAME                 quit at FRAME
-- RUN_GAME_OUT names the directory written: trace.txt (one line a frame: its number, a hash of the displayed image,
-- the program counter, the last exception's code) and shot_FRAME_WIDTHxHEIGHT_BPP.raw for each shot, the displayed
-- image as the emulator gives it.

local FNV_OFFSET = 0xcbf29ce484222325ULL
local FNV_PRIME = 0x100000001b3ULL
local BYTES_PER_WORD = 4
-- Coprocessor 0's Cause register, and its ExcCode field: the kind of the last exception taken
local CP0_CAUSE = 13
local CAUSE_CODE_SHIFT = 2
local CAUSE_CODE_MASK = 0x1f
-- The SPU's Speed setting: 1 is real time, 0 or less unbounded
local UNBOUNDED_SPEED = 0

local schedulePath = os.getenv('RUN_GAME_SCHEDULE')
local outDir = os.getenv('RUN_GAME_OUT')
if schedulePath == nil or outDir == nil then
    error('RUN_GAME_SCHEDULE and RUN_GAME_OUT must be set')
end

local presses = {}
local shots = {}
local lastFrame = nil
for line in io.lines(schedulePath) do
    local words = {}
    for word in line:gmatch('%S+') do words[#words + 1] = word end
    if words[1] == 'press' then
        local button = PCSX.CONSTS.PAD.BUTTON[words[4]]
        if button == nil then error('unknown button ' .. words[4]) end
        presses[#presses + 1] = { first = tonumber(words[2]), last = tonumber(words[3]), button = button }
    elseif words[1] == 'shot' then
        shots[tonumber(words[2])] = true
    elseif words[1] == 'end' then
        lastFrame = tonumber(words[2])
    elseif words[1] == nil then
        -- An empty line
    else
        error('unknown schedule command ' .. words[1])
    end
end
if lastFrame == nil then error('the schedule has no end') end

-- The audio sink is the emulator's clock; unbounded, it lets the emulation run as fast as the host can
PCSX.settings.spu.Speed = UNBOUNDED_SPEED

local trace = io.open(outDir .. '/trace.txt', 'w')
local pad = PCSX.SIO0.slots[1].pads[1]
local frame = 0

-- FNV-1a over the image's 32-bit words: enough to tell two images apart
local function hashImage(slice)
    local words = ffi.cast('const uint32_t *', slice.data)
    local hash = FNV_OFFSET
    for index = 0, slice.size / BYTES_PER_WORD - 1 do
        hash = bit.bxor(hash, words[index]) * FNV_PRIME
    end
    return hash
end

local function applyPresses()
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

Crash2RunGameListener = PCSX.Events.createEventListener('GPU::Vsync', function()
    frame = frame + 1
    applyPresses()
    local shot = PCSX.GPU.takeScreenShot()
    local registers = PCSX.getRegisters()
    local exceptionCode = bit.band(bit.rshift(registers.CP0.r[CP0_CAUSE], CAUSE_CODE_SHIFT), CAUSE_CODE_MASK)
    trace:write(string.format('%d %s %08x %d\n', frame, bit.tohex(hashImage(shot.data), 16), tonumber(registers.pc),
                              exceptionCode))
    if shots[frame] then
        local name = string.format('%s/shot_%d_%dx%d_%d.raw', outDir, frame, shot.width, shot.height,
                                   tonumber(shot.bpp))
        local file = Support.File.open(name, 'TRUNCATE')
        file:write(shot.data)
        file:close()
    end
    if frame >= lastFrame then
        trace:close()
        PCSX.quit(0)
    end
end)
