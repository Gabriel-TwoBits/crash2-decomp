#!/usr/bin/env python3
"""Fetches the toolchain into tools/toolchain/ at fixed versions, each download checked by its SHA-256 and each repository
checked out at its commit:

- the PSX builds of GCC (decompals/old-gcc): every version the game may have been built with (docs/COMPILER.md);
- objdiff-cli, which compares the built objects with the retail ones;
- maspsx (stands in for Sony's ASPSX assembler), m2c (the C drafts) and decomp-permuter (random rewrites until one matches).

    python tools/setup_toolchain.py

The assembler and linker are binutils for little-endian MIPS, from the system (Debian: binutils-mipsel-linux-gnu).
"""
import hashlib
import shutil
import subprocess
import sys
import tarfile
import urllib.request
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
    "maspsx": ("https://github.com/mkst/maspsx", "e85ecb373828aabea96d7e6400974d4f91ce8c74"),
    "m2c": ("https://github.com/matt-kempster/m2c", "708d2d2cb2698f091a92492b328f73b24209f72d"),
    "decomp-permuter": ("https://github.com/simonlindholm/decomp-permuter", "8556c81d80d1c1af98a858c8f4dc951357f29139"),
}
BINUTILS_PREFIX = "mipsel-linux-gnu-"
BINUTILS_TOOLS = ("as", "ld", "objcopy", "objdump")
# rwxr-xr-x: the downloaded programs run
EXECUTABLE_MODE = 0o755


def download(url, target, sha256):
    if not target.exists():
        print(f"downloading {url}", flush=True)
        target.parent.mkdir(parents=True, exist_ok=True)
        partial = target.with_suffix(target.suffix + ".part")
        with urllib.request.urlopen(url) as response, open(partial, "wb") as out:
            shutil.copyfileobj(response, out)
        partial.rename(target)
    actual = hashlib.sha256(target.read_bytes()).hexdigest()
    if actual != sha256:
        sys.exit(f"{target}'s SHA-256 is {actual}, not {sha256}")


def old_gcc(version, sha256):
    folder = TOOLCHAIN / f"gcc-{version}-psx"
    if (folder / "cc1").exists():
        return
    archive = TOOLCHAIN / "downloads" / f"gcc-{version}-psx.tar.gz"
    download(OLD_GCC_URL.format(release=OLD_GCC_RELEASE, version=version), archive, sha256)
    folder.mkdir(parents=True, exist_ok=True)
    with tarfile.open(archive) as tar:
        tar.extractall(folder, filter="data")


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

    missing = [BINUTILS_PREFIX + tool for tool in BINUTILS_TOOLS if shutil.which(BINUTILS_PREFIX + tool) is None]
    if missing:
        print(f"missing {', '.join(missing)}: sudo apt install binutils-mipsel-linux-gnu (or your distribution's)")
        sys.exit(1)
    print(f"toolchain ready in {TOOLCHAIN}")


if __name__ == "__main__":
    main()
