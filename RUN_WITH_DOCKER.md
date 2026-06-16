# Legend of Zelda GBA Mini-Game: Docker Run Guide

This project is a Game Boy Advance mini-game boss fight between Link and Ganon. The game is written in C and is built with the CS 2110 GBA toolchain inside Docker. The included Makefile builds the ROM as `App.gba` and launches it with mGBA through the CS 2110 Docker environment.

## Prerequisites

Before running the game, make sure you have:

- Docker Desktop installed and running.
- A terminal that can run shell scripts:
  - macOS or Linux Terminal works by default.
  - Windows users should use Git Bash, WSL, or another Bash-compatible terminal.
- The project folder extracted from the `.zip` file.

## Project Files Used

The main files involved in running the game are:

- `Makefile` — builds the GBA ROM and provides the `make mgba` command.
- `cs2110docker-gba.sh` — starts the CS 2110 GBA Docker container.
- `GBAServer.jar` — helps connect the Docker container to the mGBA emulator on your host machine.
- `App.gba` — the generated GBA ROM file.

## 1. Unzip the Project

If you have not already extracted the project, unzip it first:

```bash
unzip CS2110_LOZ.zip
cd CS2110_LOZ
```

All commands below should be run from inside the `CS2110_LOZ` project folder.

## 2. Start Docker

Open Docker Desktop and make sure it is fully running before continuing.

You can verify Docker is available with:

```bash
docker -v
```

If Docker is running correctly, this should print your Docker version.

## 3. Make the Docker Script Executable

The helper script may need executable permissions. Run:

```bash
chmod +x cs2110docker-gba.sh
```

## 4. Start the CS 2110 GBA Docker Container

From inside the project folder, run:

```bash
./cs2110docker-gba.sh start
```

This script will:

1. Pull the CS 2110 GBA Docker image: `gtcs2110/cs2110docker-c:stable-gba`.
2. Start `GBAServer.jar` on your host machine.
3. Mount your current project folder into the Docker container at `/cs2110/host/`.
4. Open an interactive terminal inside the Docker container.

After the container starts, your terminal prompt should change to indicate that you are now inside Docker.

## 5. Navigate to the Mounted Project Folder

Inside the Docker container, run:

```bash
cd /cs2110/host
```

This folder contains the same files as your local `CS2110_LOZ` project folder.

## 6. Build and Run the Game

Inside Docker, run:

```bash
make mgba
```

The Makefile will:

1. Clean old compiled files.
2. Compile the C source files.
3. Link the object files into `App.elf`.
4. Convert `App.elf` into the GBA ROM file `App.gba`.
5. Launch the game in mGBA.

If everything works, the mGBA emulator should open and display the game start screen.

## Controls

| Action | Key |
| --- | --- |
| Start normal mode | Enter |
| Move Link | Arrow keys |
| Shoot the Master Sword | A button, usually `x` on Mac keyboards in mGBA |
| Restart / return to start screen | Backspace |
| Start hard mode after unlocking it | A button |

## Gameplay Goal

Defeat Ganon before the timer reaches zero.

You lose if:

- Link touches Ganon.
- Link runs out of hearts.
- The timer reaches zero.

You win by shooting the Master Sword at Ganon until his health reaches zero.

After winning, you can replay in normal mode or hard mode. Hard mode gives Link less health, makes Ganon and the fireballs faster, and starts the timer at 30 seconds.

## Useful Makefile Commands

Run these commands from inside `/cs2110/host` in the Docker container.

### Build the ROM

```bash
make
```

### Build and launch in mGBA

```bash
make mgba
```

### Remove compiled files

```bash
make clean
```

### Build with debug symbols

```bash
make debug
```

## Stop the Docker Container

When you are finished, exit the Docker container:

```bash
exit
```

Then, from your host machine inside the project folder, run:

```bash
./cs2110docker-gba.sh stop
```

This stops and removes any running CS 2110 Docker containers.

## Troubleshooting

### Docker is not found

If you see an error saying Docker is not found, install Docker Desktop and restart your terminal.

### Docker is not running

If you see an error saying Docker is not currently running, open Docker Desktop and wait until it finishes starting.

### Permission denied when running the script

Run:

```bash
chmod +x cs2110docker-gba.sh
```

Then try again:

```bash
./cs2110docker-gba.sh start
```

### `make mgba` does not open the emulator

Check that:

- You started the container using `./cs2110docker-gba.sh start`.
- You are running `make mgba` inside Docker, not on your host machine.
- You are inside `/cs2110/host` before running `make mgba`.
- Docker Desktop is still running.

You can also check whether an emulator log was created:

```bash
ls *.log
```

### The game does not rebuild correctly

Clean the project and rebuild:

```bash
make clean
make mgba
```

## Quick Command Summary

From your host machine:

```bash
unzip CS2110_LOZ.zip
cd CS2110_LOZ
chmod +x cs2110docker-gba.sh
./cs2110docker-gba.sh start
```

Inside Docker:

```bash
cd /cs2110/host
make mgba
```

When finished, exit Docker and stop the container:

```bash
exit
./cs2110docker-gba.sh stop
```
