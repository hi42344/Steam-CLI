# Steam-CLI
I don't like using the UI much, and this is useful to me

# How To Use

- **Steam must be open in the background or you will have to go log in (at least for me when I tried)**
- **All commands that take a game name that name can be multiple args**
- **All commands that take a game name are case-insensitive**
- **Will probably open steam, so keep that in mind**
- **Line seperator is "-------------"**

## Commands

- `run` -> `steam run GAME NAME`: **Runs a game**
- `info` -> `steam info GAME NAME`: **Displays info about a game or all games (ex: "Name:         Counter-Strike 2 AppID:        730 Size on Disk: 66.66391980 GB Path:         c:/program files (x86)/steam\steamapps\common\Counter-Strike Global Offensive" (for 'steam info --all' it displays all of your games stats like the CS2 example (with line seperators)))**
- `list` -> `steam list`: **Lists all installed game names with their names (with line seperators)**
- `verify` -> `steam verify GAME NAME`: **Verifys a game**
- `uninstall` -> `steam uninstall GAME NAME`: **Uninstalls a game**
- `install` -> `steam install GAME NAME/APP ID`: **Installs a game from either a app id or a name, if a name it gives a list to pick from (if any results)**