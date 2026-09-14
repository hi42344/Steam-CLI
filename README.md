# Steam-CLI
My first open-source project

# License

- The **Steam-CLI** application as a whole is licensed under the [GNU General Public License v3.0](LICENSE).
- Standalone utility headers ([`vdf_parser.hpp`](src/vdf_parser.hpp) and [`Colors.hpp`](src/helpers/Colors.hpp)) are dual-licensed under the **MIT License** and can be reused individually in any project without GPL restrictions.

# How To Use

- **Steam must be open in the background or you will have to go log in (at least for me when I tried)**
- **All commands that take a game name that name can be multiple args**
- **All commands that take a game name are case-insensitive**
- **Will probably open steam, so keep that in mind**
- **Line seperator is "-------------"**

## Commands

### Built-in Commands

- `list` -> `steam list`: **Lists all installed games separated by line dividers**
- `info` -> `steam info GAME NAME` / `steam info --all`: **Displays size, AppID, and install paths for a game or all installed games**
- `search` -> `steam search QUERY`: **Searches the Steam Store API online for games and displays matching results**
- `path` -> `steam path GAME NAME`: **Prints and opens the game folder in File Explorer (running `steam path` alone opens the main Steam folder)**
- `config` -> `steam config`: {
    - `open` -> `steam config open`: **Opens the config/Commands.json file in your default text editor**

}

### Standard Config Commands

**These default commands come pre-configured in `config/Commands.json` and can be customized or extended if needed**

#### Game Commands

- `run` -> `steam run GAME NAME / APPID`: **Launches a game**
- `verify` -> `steam verify GAME NAME / APPID`: **Validates game files**
- `install` -> `steam install GAME NAME / APPID`: **Prompts Steam to install a game**
- `uninstall` -> `steam uninstall GAME NAME / APPID`: **Prompts Steam to uninstall a game**
- `backup` -> `steam backup GAME NAME / APPID`: **Opens the backup wizard for a game**
- `news` -> `steam news GAME NAME / APPID`: **Opens the news hub for a game**
- `achievements` -> `steam achievements GAME NAME / APPID`: **Opens community achievements for a game**
- `workshop` -> `steam workshop GAME NAME / APPID`: **Opens the Steam Workshop hub for a game**
- `community` -> `steam community GAME NAME / APPID`: **Opens the game's Community hub**
- `store` -> `steam store GAME NAME / APPID`: **Opens the game's Steam Store page**
- `guides` -> `steam guides GAME NAME / APPID`: **Opens community guides for a game**
- `discussions` -> `steam discussions GAME NAME / APPID`: **Opens community forums for a game**
- `db` -> `steam db GAME NAME / APPID`: **Opens the game's entry on SteamDB**
- `artwork` -> `steam artwork GAME NAME / APPID`: **Opens community artwork and screenshots for a game**
- `stats` -> `steam stats GAME NAME / APPID`: **Opens personal stats for a game**
- `support` -> `steam support GAME NAME / APPID`: **Opens Steam Support for a specific game**

#### Other

- `settings` -> `steam settings`: **Opens Steam settings**
- `downloads` -> `steam downloads`: **Opens the Steam downloads manager**
- `library` -> `steam library`: **Switches Steam view directly to your game library**
- `friends` -> `steam friends`: **Opens your Steam Friends list**
- `inventory` -> `steam inventory`: **Opens your Steam inventory**
- `console` -> `steam console`: **Opens the native Steam client developer console**
- `status` -> `steam status`: **Opens SteamStat.us to check Steam network and service operational status**
- `uri` -> `steam uri URL_OR_COMMAND`: **Executes a raw URI, automatically prefixing `steam://` if no protocol is supplied**

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