# Steam-CLI
My first open-source project

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
- `search` -> `steam search GAME NAME/APP ID`: **Searchs for a game and gives a list of results (if any)**
- `backup` -> `steam backup GAME NAME`: **Opens the backup wizard for a game**
- `news` -> `steam news GAME NAME`: **Opens the news for a game (checks offline or online (online if game not found locally))**
- `achievements` -> `steam achievements GAME NAME`: **Opens the achievements of a game on the web (locally or online)**
- `path` -> `steam path GAME NAME`: **Prints the game's path and opens it in file explorer (bare `steam path` prints steam's path and opens it in file explorer)**
- `workshop` -> `steam workshop GAME NAME`: **Opens a game workshop (locally installed or online) on the web**
- `uri` -> `steam uri URL_OR_COMMAND`: **Executes a raw uri, auto adding the 'steam://' prefix if no http://, https://, or steam:// found**

## Custom Commands (`config/Commands.json`)

**You can create custom commands by defining them in `config/Commands.json`**

### Configuration Schema

**Add entries to `config/Commands.json` using the following format: (almost all the things all optional, these just show everything for clarity)**

```json
{
  "community": {
    "uri": "[https://steamcommunity.com/app/@APP_ID](https://steamcommunity.com/app/@APP_ID)",
    "local": true,
    "online": true,
    "print-message": "Opening community hub for @GAME_NAME...",
    "Needs-game_name/appid-arg": true
  },
  "settings": {
    "uri": "open/settings",
    "local": false,
    "online": false,
    "print-message": "Opening Steam settings...",
    "Needs-game_name/appid-arg": false
  }
}
```