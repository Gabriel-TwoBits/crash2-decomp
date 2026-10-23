#!/usr/bin/env python3
"""Fetches the toolchain into tools/toolchain/ at fixed versions, each download checked by its SHA-256 and each repository
checked out at its commit:

- the PSX builds of GCC (decompals/old-gcc): every version the game may have been built with (docs/COMPILER.md);
- objdiff-cli, which compares the built objects with the retail ones;
- maspsx (stands in for Sony's ASPSX assembler), m2c (the C drafts) and decomp-permuter (random rewrites until one matches);
- PsyQ's library signatures, which tools/find_psyq.py looks for in the executable;
- PCSX-Redux, the emulator tools/run_game.py runs the game in (its AppImage, extracted).

    python tools/setup_toolchain.py

The assembler and linker are binutils for little-endian MIPS, from the system (Debian: binutils-mipsel-linux-gnu).
"""
import hashlib
import shutil
import subprocess
import sys
import tarfile
import urllib.request
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]
TOOLCHAIN = HERE / "tools" / "toolchain"
OLD_GCC_RELEASE = "0.17"
OLD_GCC_URL = "https://github.com/decompals/old-gcc/releases/download/{release}/gcc-{version}-psx.tar.gz"
# PsyQ's GCC versions around the game's 1997 libraries; the Crash Bandicoot 1 decomp needed 2.5.7 and 2.6.3
OLD_GCC_SHA256 = {
    "2.5.7": "0db1fb403c766e51b094354b9a136a6c1f01457aa41ccca28ac457b7cb67d871",
    "2.6.0": "358dac949f0f566af9c6af93bba0d4ac04d56ca9df80c58bd66d81bbb50deceb",
    "2.6.3": "01e6e8c4933414ea3f8d8e3bc766a1f5fafd4fc0110e0b75d1f691bd791989b1",
    "2.7.2": "500a459b3485e885a8d302cac23c2a4632f3900e03a09153f6190699fd723571",
    "2.8.1": "f6f6e883942d4d3289d048236c672e71ed410e546aaae8ff655952f1567e1be0",
}
OBJDIFF_VERSION = "v3.8.2"
OBJDIFF_URL = f"https://github.com/encounter/objdiff/releases/download/{OBJDIFF_VERSION}/objdiff-cli-linux-x86_64"
OBJDIFF_SHA256 = "e5445089a6f707e30cb5e02992451b604619cedfd2660ee2373a86bfefd9e718"
REPOSITORIES = {
    "maspsx": ("https://github.com/mkst/maspsx", "d4661dfd0fad27fad4cab5781f18697b823369f9"),
    "m2c": ("https://github.com/matt-kempster/m2c", "708d2d2cb2698f091a92492b328f73b24209f72d"),
    "decomp-permuter": ("https://github.com/simonlindholm/decomp-permuter", "8556c81d80d1c1af98a858c8f4dc951357f29139"),
    # Byte signatures of PsyQ's library objects, per SDK version (tools/find_psyq.py); no licence, so fetched, never vendored
    "psx_psyq_signatures": ("https://github.com/lab313ru/psx_psyq_signatures", "e9e46e7e133ef275a79bfce650924f98edb086bc"),
}
# PCSX-Redux has no releases: its builds are published at distrib.app, one zip holding the AppImage per build; this is
# build 436 of the dev channel, commit 8d405b66. Its 2024 build 124 crashes on this game within 10 seconds.
# TRADEOFF: the dev channel keeps about its last 100 builds (three weeks); once this one is gone, pin a newer one from
# https://distrib.app/storage/manifests/pcsx-redux/dev-linux-x64/manifest.json (its manifest-<id>.json gives the path)
PCSX_REDUX_URL = ("https://distrib.app/storage/assets/18b/f1f/2b3/3350a8c62bc975807e58429625d0380a679c8181cc1840fea16cc4f/"
                  "PCSX-Redux-8d405b66-linux-x86_64.zip")
PCSX_REDUX_SHA256 = "690889d6053b5e5008e829ef21733fb983d4aae2ab42b962b75ac65c2b453fc5"
PCSX_REDUX_APPIMAGE = "PCSX-Redux-HEAD-x86_64.AppImage"
PCSX_REDUX = TOOLCHAIN / "pcsx-redux"
PCSX_REDUX_BINARY = PCSX_REDUX / "usr" / "bin" / "pcsx-redux"
# The folder an AppImage extracts itself into, in the working directory
APPIMAGE_EXTRACTED = "squashfs-root"
BINUTILS_PREFIX = "mipsel-linux-gnu-"
BINUTILS_TOOLS = ("as", "ld", "nm", "objcopy", "objdump")
# distrib.app refuses Python's default User-Agent
USER_AGENT = "crash2-decomp-setup"
# rwxr-xr-x: the downloaded programs run
EXECUTABLE_MODE = 0o755


def download(url, target, sha256):
    """The file at url as target, which only a download of that SHA-256 becomes"""
    if target.exists():
        checked = target
    else:
        print(f"downloading {url}", flush=True)
        target.parent.mkdir(parents=True, exist_ok=True)
        checked = target.with_suffix(target.suffix + ".part")
        request = urllib.request.Request(url, headers={"User-Agent": USER_AGENT})
        with urllib.request.urlopen(request) as response, open(checked, "wb") as out:
            shutil.copyfileobj(response, out)
    actual = hashlib.sha256(checked.read_bytes()).hexdigest()
    if actual != sha256:
        if checked != target:
            checked.unlink()
        sys.exit(f"{url}'s SHA-256 is {actual}, not {sha256}")
    elif checked != target:
        checked.rename(target)
    else:
        # Downloaded before, and still the same
        pass


def old_gcc(version, sha256):
    folder = TOOLCHAIN / f"gcc-{version}-psx"
    if (folder / "cc1").exists():
        return
    archive = TOOLCHAIN / "downloads" / f"gcc-{version}-psx.tar.gz"
    download(OLD_GCC_URL.format(release=OLD_GCC_RELEASE, version=version), archive, sha256)
    # Extracted aside and moved in whole: a folder with cc1 is a complete one
    work = TOOLCHAIN / f"gcc-{version}-psx.part"
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    with tarfile.open(archive) as tar:
        tar.extractall(work, filter="data")
    if folder.exists():
        shutil.rmtree(folder)
    work.rename(folder)


def repository(name, url, commit):
    folder = TOOLCHAIN / name
    if not folder.exists():
        print(f"cloning {url}", flush=True)
        subprocess.run(["git", "clone", "--quiet", url, str(folder)], check=True)
    head = subprocess.run(["git", "-C", str(folder), "rev-parse", "HEAD"], capture_output=True, text=True,
                          check=True).stdout.strip()
    if head != commit:
        subprocess.run(["git", "-C", str(folder), "fetch", "--quiet", "origin"], check=True)
        subprocess.run(["git", "-C", str(folder), "checkout", "--quiet", "--detach", commit], check=True)


def pcsx_redux():
    """The AppImage out of its checked zip, extracted by itself: running the extracted binary needs no FUSE"""
    if PCSX_REDUX_BINARY.exists():
        return
    archive = TOOLCHAIN / "downloads" / PCSX_REDUX_URL.rsplit("/", 1)[1]
    download(PCSX_REDUX_URL, archive, PCSX_REDUX_SHA256)
    work = TOOLCHAIN / "pcsx-redux.part"
    if work.exists():
        shutil.rmtree(work)
    work.mkdir()
    with zipfile.ZipFile(archive) as zipped:
        zipped.extract(PCSX_REDUX_APPIMAGE, work)
    appimage = work / PCSX_REDUX_APPIMAGE
    appimage.chmod(EXECUTABLE_MODE)
    subprocess.run([str(appimage), "--appimage-extract"], cwd=work, stdout=subprocess.DEVNULL, check=True)
    if PCSX_REDUX.exists():
        shutil.rmtree(PCSX_REDUX)
    (work / APPIMAGE_EXTRACTED).rename(PCSX_REDUX)
    shutil.rmtree(work)
    if not PCSX_REDUX_BINARY.exists():
        sys.exit(f"{PCSX_REDUX_APPIMAGE} has no usr/bin/pcsx-redux")


def main():
    if shutil.which("git") is None:
        sys.exit("git isn't installed")
    for version, sha256 in OLD_GCC_SHA256.items():
        old_gcc(version, sha256)
    objdiff = TOOLCHAIN / "objdiff-cli"
    download(OBJDIFF_URL, objdiff, OBJDIFF_SHA256)
    objdiff.chmod(EXECUTABLE_MODE)
    for name, (url, commit) in REPOSITORIES.items():
        repository(name, url, commit)
    pcsx_redux()

    missing = [BINUTILS_PREFIX + tool for tool in BINUTILS_TOOLS if shutil.which(BINUTILS_PREFIX + tool) is None]
    if missing:
        print(f"missing {', '.join(missing)}: sudo apt install binutils-mipsel-linux-gnu (or your distribution's)")
        sys.exit(1)
    print(f"toolchain ready in {TOOLCHAIN}")


if __name__ == "__main__":
    main()
