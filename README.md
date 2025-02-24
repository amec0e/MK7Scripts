# MK7Scripts
Various Scripts I made with prompt engineering for the WiFi Pineapple MK7.

## DISCLAIMER:

Please keep in mind these are not entirely well crafted. There may also be potential bugs, however they perform the operations for the intended tasks!

Please **use at your own risk!**

Myself (amec0e) assumes NO liability or responsibility for any misuse or unintended consequences resulting from the use of these tools.

These tools are provided with the expectation that users will comply with all applicable laws and regulations. They are intended for educational and research purposes only.

**Please note:** Myself (amec0e) **DOES NOT** provide support for these tools.

## Contents:

- [Macspoof](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#macspoof)
- [Miniairo](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#miniairo)
- [gather_probes](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#gather_probes)
- [sort_probes](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#sort_probes)
- [deauther](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#deauther)
- [bpineap](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#bpineap)
- [check_handshakes](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#check_handshakes)
- [airedeauth](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#airedeauth)
- [capture_handshakes](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#capture_handshakes)
- [wpa2clean](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#wpa2clean)
- [pcap_split](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#pcap_split)
- [Process_MAL_Only.py](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#process_mal_onlypy)
- [Process_MLA_Complete.py](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#process_mla_completepy)


**Tip:** If you want to be able to tab autocomplete the commands, just put them in `/bin/`, this will allow you to autocomplete the command by pressing the tab key.

## Macspoof:

uses macchanger -r for random or allows manual input. Three options to choose from, wlan1, wlan3 (if you have the MK7AC adapter or compatible card), wlan2.

This also uses `monitor_vif` to ready the virtual interfaces how pineapple does when you select a recon interface from within the pineapples webUI, so you have wlan3 and wlan3mon interfaces instead of just a single one.


**Usage:**
```bash
./macspoof
```

- Select Option 1 and Either enter MAC or press Enter for random generation.
- Select Option 2 and Either enter MAC or press Enter for random generation.
- Select Option 3 and Either enter MAC or press Enter for random generation. **(CLIENT INTERFACE)**


## Miniairo:

This is a little wrapper around the `airodump-ng` command as there was options that I like to use often (uptime, manufacturer, wps) but wanted to be a little lazy and not have to type out all the commands in their entirety every time. Basic usage is `./miniairo -i <interface> -c channel`, there is a help menu.

**Usage:**
```bash
./miniairo -i wlan1mon -b BSSID -c 7
```

**Help Menu:**
```
Usage: ./miniairo -i <interface> [options]
Options:
  -i <INTERFACE>    Mandatory: Specify interface name.
  -b <BSSID>        Optional: Specify the BSSID.
  -c <channel>      Optional: Specify the channel.
  -u                Optional: Enable uptime information.
  -m                Optional: Enable manufacturer information.
  -w                Optional: Enable WPS information.
  -W <prefix>       Optional: Specify the output file prefix.
  -e <ESSID>        Optional: Specify the ESSID. Use quotes for names with spaces.
  -B <band>         Optional: Specify the band (abg).
  -T <TIMER>        Optional: Exit the program after TIMER seconds.
  -h, --help        Display this help message.
```


## gather_probes:

**requires:** `sqlite3-cli`, `libsqlite3`
**Install:** `opkg install libsqlite3`, `opkg install sqlite3-cli`

This takes the activity log.db, copies this to tmp and extracts the ESSID and BSSIDs from the log using sqlite3-cli. This then uses `sort` and `uniq` on it and outputs a file called probes1.txt (this creates a new directory called gprobes in root and increments the output file names).  You can also exclude ESSIDs using a input file containing ESSIDs to exclude (one per line).

This is useful to check for potential karma attack victims as well as new SSIDs perhaps not in your SSID Pool.

**Usage:**
```bash
./gather_probes
```

**Help Menu:**

```
Usage: ./gather_probes [-e <input_file>]
Options:
  -e, --exclude-file  Specify the input file containing SSIDs to exclude (each SSID on a separate line)
  -h, --help          Display this help menu
```

**NOTE:** If you have an issue using `gather_probes` ensure you have installed `sqlite3-cli` and that your `libsqlite3` is the same version. 
```bash
opkg update
opkg install libsqlite3
```


## sort_probes:

This is similar to the above except it combines the probes, it uses `sort` and `uniq` on all probe* files within the directory (/root/gprobes/probes*), this sorts multiple probe outputs into one so you can combine your list of target probes. 

If you have multiple (which `gather_probes.sh` will do), it will output and overwrite the file called `sorted_probes.txt` so ensure you do not clear all your probes unless you want to. You could also rename `sorted_probes.txt` to `probes_99.txt` and add that to gprobes before sorting again. If you need to exclude ESSIDs, exclude them with `gather_probes`.

**Usage:**
```bash
./sort_probes
```


## Process_MAL_Only.py:

**requires:** `python3-unidecode`
**install:** `opkg install python3-unidecode`

This script takes the Maclookup.app CSV database and extracts and sorts all of the MAL addresses only (first 3 octets/pairs). This is also used for the WiFi Pineapples main recon

You can check the output files key pairing count using: `jq -c 'keys_unsorted | length' youfile.json`

**Usage:**
```python
python3 ./Proccess_MAL_Only.py -i input.csv -o output.json
```

**Upgrading:**

Once you have your upgraded file and it is on your pineapple, simply replace the file and reboot your pineapple.

1. Replace: `mv /root/output.json /etc/pineapple/ouis`
2. Reboot Pineapple: `sync` , `reboot`

**NOTE:** This is best run on a normal desktop or laptop, I would not try to run this on the Pineapple Itself.


## Process_MLA_Complete.py:

**requires:** `python3-unidecode`
**install:** `opkg install python3-unidecode`

This works exactly the same as the above except it will extract everything and this is then used for the MACInfo module. Just ensure that when replacing the MACInfo `MLA_OUI_COMPLETE` that you rename it exactly that.

**Usage:**
```python
python3 ./Process_MLA_Complete.py -i input.csv -o output.json
```

**To Upgrade MACInfo Module:**

1. Remove the module from the modules tab in the WebUI as you normally would using the trashcan icon.
2. Using Web Terminal run: `rm -rf /root/.MACInfo/`
3. Reboot Pineapple. **(Important)**
4. Extract the MACInfo directory from within the archive and change into the MACInfo directory.
5. Replace your MLA_OUI_COMPLETE file with your Updated one named exactly the same.
6. Repackage the directory as the extension tar.gz (I used 7z to package as a tar first and then as a gz).
7. Sideload the module and then attempt to search a mac address (or just click search). This will perform the setup and relocation of the MLA_OUI_COMPLETE file to `/root/.MACInfo/`.

**NOTE:** This is best run on a normal desktop or laptop, I would not try to run this on the Pineapple Itself.


## deauther:

This is a wrapper around mdk4 to deauthenticate, the magic is you can specify a station file of BSSIDs or a list of AP BSSIDs with channel numbers in a list and it will change the channel accordingly per target. It allows you to set a run time duration, how long to wait between attack attempts, how many times to repeat the attack on a target and allows you to set the packets per second.

**Usage:**
```bash
./deauther -i wlan1mon -B AP_BSSID.txt -d DURATION -w WAIT -r REPEATS --ps PACKET_SPEED
```

**Help Menu:**
```
Usage: deauther [-i INTERFACE] [-c CHAN] [-b BSSID] [-s STATION] [-B AP_BSSID_FILE] [-S STATION_BSSID_FILE] [-d DURATION] [-w WAIT] [-r REPEATS] [-p SPEED] [-h]


Options:
  -i INTERFACE    Specify the interface name to use.
  -c CHAN         Specify the channel number (overrides when using a file).
  -b BSSID        Specify the target BSSID (mandatory for single target).
  -s STATION      Specify the target STATION BSSID (mandatory for single station).
  -B FILE         Specify the path to the file containing target BSSIDs and channels (one per line, format: BSSID,CHANNEL).
  -S FILE         Specify the path to the file containing target STATION BSSIDs and channels (one per line, format: BSSID,CHANNEL).
  -d DURATION     Specify the duration of mdk4 runtime in seconds (default: 25).
  -w WAIT         Specify the delay between runs in seconds (default: 60).
  -r REPEATS      Specify how many times the cycle should repeat (default: 3).
  --ps SPEED      Specify the packets per second (default: unlimited).
  -u              Update -B BSSID Channels with Prompting.
  --au            Update -B BSSID Channels without Pompting.
  --st TIME       Specify channel update duration of --au.
  --kf            Keep the temporary file made for -B or -S after use instead of deleting.
  --am MODE       Specify Repeating Mode. 1 = Single Target, Full Repeats | 2 = All Targets, Full Repeats (default).
  -h              Show this help menu.

Default Durations:
  DURATION       30 seconds
  WAIT           30 seconds
  REPEATS        1 times
  SPEED          0 (0 = unlimited)
  MODE           2

NOTE ON MODE:
  1 - Single Target, Full Repeats Per Target, In Order
  2 - All Targets, Full Repeats, In Order

Target File Format for BSSID File:

  BSSID,CHAN
  00:11:22:33:44:55,13

Target File Format for Station BSSID File (Most Effective):

  BSSID,CHAN
  00:11:22:33:44:55,10

Examples:

Single Target AP, Single Channel:
  deauther -i wlan1mon -c 6 -b 00:11:22:33:44:55

Single Target Station, Single Channel:
  deauther -i wlan1mon -s 00:11:22:33:44:55 -c 10

AP BSSID File, With Optional Defaults:
  deauther -i wlan3mon -B targets.txt -d 20 -w 30 -r 5

AP BSSID File, Defaults Only:
  deauther -i wlan1mon -B targets.txt

Station BSSID file, Defaults:
  deauther -i wlan1mon -S stations.txt

Station BSSID file (Quick Sweep):
  deauther -i wlan1mon -S stations.txt -d 11 -w 5 -r 1
```


## bpineap:

This allows you to adjust all of the options you would find using `uci show pineap` minus the `ap_interface` as this just does not work correctly due to other factors at play. This uses `uci` to set these options temporarily and then restarts pineapd to ensure the changes take affect. You can also show a scan, stop a scan and start a scan using the cli `pineap` options. It just saves time typing or copy and pasting the uci line to edit.

**Usage:**
```bash
./bpineap pineap_interface wlan1mon
```

**Help Menu:**
```
Usage: ./bpineap <option> <value>

NOTICE: These changes made are not permanent in case of a error
 a reboot will reset these changes. If you want to make them permanent
 then use 'uci commit' this will commit ALL changes made!


Options:
  karma                      [on or off]
  beacon_interval            [LOW, NORMAL or AGGRESSIVE]
  beacon_response_interval   [LOW, NORMAL or AGGRESSIVE]
  beacon_responses           [on or off]
  broadcast_ssid_pool        [on or off]
  broadcast_ssid_pool_random [on or off]
  mac_filter                 [black or white]
  ssid_filter                [black or white]
  target_mac                 <MAC_ADDRESS>
  handshakes_path            <PATH>
  ssid_db_path               <PATH>
  filters_db_path            <PATH>
  connect_notifications      [on or off]
  disconnect_notifications   [on or off]
  auto_ssid_filter           [on or off]
  auto_mac_filter            [on or off]
  pineape_passthrough        [on or off]
  hostapd_db_path            <PATH>
  recon_db_path              <PATH>
  pineap_mac                 <MAC_ADDRESS>
  evil_ap_handshakes         [on or off]
  capture_ssids              [on or off]
  logging                    [on or off]
  autostart                  [on or off]
  pineap_interface           [wlan1mon or wlan3mon]
  run_scan [duration sec (0 = Continuous)] [Band: 0,1,2 (2.4GHz,5GHz,Both)] [capture handshakes: 0,1 (off,on)]
  show_scan                  (Show current running scan, if any)
  stop_scan                  (Stop current running scan, if any)
  show_config                (Show curent uci config)
  clear_logs                 (Clears activity log [log.db])
  start_handshake_capture    [bssid] [channel]
  stop_handshake_capture     (Stop current handshake capture, if any)
  deauth                     [AP MAC] [Client MAC] [Channel] [Count]
```

## airedeauth:

This is similar to deauther except this uses `aireplay-ng` to perform the deauthentication, this is more useful for more defined targeting.

**Usage:**
```
./airedeauth -i wlan1mon -b 00:11:22:33:44:55 -s aa:bb:cc:dd:ee:ff -c 6
```

**Help Menu:**
```
Usage: airedeauth [-i INTERFACE] [-b AP_BSSID] [-c CHANNEL] [-s STA_BSSID] [-f AP_BSSID_FILE] [-w WAIT] [-r REPEATS] [--pc COUNT] [--ps SPEED] [--rc REASON] [--sd STADURATION] [--st SCANTIME] [--fu FILTERUNASSOC] [--fbd FILTERBYDATA] [--wint WRITEINT] [--uch] [--ag] [--an] [--aq] [--kf] [-h] [--help]

Options:
  -i INTERFACE      Specify the interface name to use.
  -b AP_BSSID       Specify the target AP BSSID (mandatory for single target if not using -f).
  -c CHANNEL        Specify the channel number (mandatory if not using -f).
  -s STA_BSSID      Specify the target STA BSSID (mandatory for single target if not using -f).
  -f FILE           Specify the path to the file containing target AP BSSIDs and channels (one per line, format: BSSID,STATION,CHAN).
  -w WAIT           Specify the delay between runs in seconds (default: 30).
  -r REPEATS        Specify how many times the cycle should repeat (default: 1).
  --pc COUNT        Specify the number of deauthentication packet groups to send per run (default: 10).
  --ps SPEED        Specify the packet speed (default: 5).
  --rc REASON       Specify the deauthentication reason code (default: 0).
  --sd STADURATION  Specify Duration for autogathering of stations. (default: 40
  --ag              Autogather stations only. (MUST use --kf or the file will be deleted.)
  --uch             Update AP BSSID Files Channels only.
  --an              Auto Update Channels and Stations + Attack (supply file format: BSSID,,)
  --aq              Auto Gather Stations + Attack.
  --st SCANTIME     Specify the Scantime for channel update (default: 38).
  --fu true         Specify Filter Unassociated client for channel update (default: true).
  --fbd INT         Specify Filter AP By Data Recieved for channel update (default: 0).
  --wint INT        Specify the write interval for channel update (default: 1).
  --kf              Keep the generated file for --ag, --an, --aq or normal attack after use instead of deleting.
  -h                Show this help menu.
 --help             Same as -h.

Default Values:
  WAIT              30 seconds
  REPEATS           1 times
  COUNT             10 packets
  SPEED             5 packets/second
  REASON            0 reason code
  STADURATION       40 seconds
  AUTOGATHER        false
  WRITEINT          1
  FILTERBYDATA      0
  FILTERUNASSOC     true
  SCANTIME          38
  SCANTIME (AUTO)   65

Reason Codes:
  1 - Unspecified reason.
  2 - Previous authentication no longer valid.
  3 - Deauthenticated because sending station (STA) is leaving or has left Independent Basic Service Set (IBSS) or ESS.
  4 - Disassociated due to inactivity.
  5 - Disassociated because WAP device is unable to handle all currently associated STAs.
  6 - Class 2 frame received from nonauthenticated STA.
  7 - Class 3 frame received from nonassociated STA.
  8 - Disassociated because sending STA is leaving or has left Basic Service Set.
  9 - STA requesting (re)association is not authenticated with responding STA.

Target File Format for AP BSSID File (Autogather):
NOTE: ,, IS A PLACEHOLDER DO NOT REMOVE IT

  BSSID,,CHAN
  DE:AD:BE:EF:12:34,,6

Target File Format for AP BSSID File (Manual Gathering):

  BSSID,STATION,CHANNEL
  00:11:22:33:44:55,66:11:22:33:55:44,10

Auto All File format (update/get channels, gather stations, attack):
  BSSID,,
  00:11:22:33:44:55,,


Examples:

Single Target AP, Single Channel:
  airedeauth -i wlan1mon -b 00:11:22:33:44:55 -c 6

Single Target AP and Target STA, Single Channel:
  airedeauth -i wlan1mon -b 00:11:22:33:44:55 -s aa:bb:cc:dd:ee:ff -c 6

AP BSSID File, With Optional Defaults:
  airedeauth -i wlan3mon -f targets.txt -w 30 -r 5

AP BSSID File: Autogather
  airedeauth -i wlan1mon -f ap-channel-only.txt --ag
```


## capture_handshakes

**requires:** `coreutils-shuf`
**install:** `opkg install coreutils-shuf`

This uses a simple loop to take a input list of BSSIDs and Channel numbers and one by one starts `pineap handshake_capture_start` and `pineap handshake_capture_stop`. This allows you to start a dedicated handshake capture the same way you would if using the WebUI, to simply stop and start capturing handshakes for different BSSIDs on different channels. Use this with `screen` and you can start a 24 hour handshake capture spree targeting selected access points on their corresponding channels. You can only run one instance of this as it will conflict with other scans otherwise and there might be some unintended behaviour.

**Usage:**
```
./capture_handshakes -f bssids.txt -t 3600 -i iface -m 2
```

**Help Menu:**
```
Usage: capture_handshakes [-h] [-f FILE] [-t TIMER] [-i INTERFACE] [-m MODE] [-s] [-u] [--kf]

Options:
  -h             Display this help menu.
  -f FILE        Input file containing BSSIDs and channels.
  -t TIMER       Time to capture per target in seconds.
  -s             Shuffle lines in the input file.
  -u             Update BSSID channels.
  -m MODE        Capture mode: 1 = Pineap, 2 = Airodump-ng (default: 2).
  -i INTERFACE   Specify interface for mode 1 or 2 (not needed for -m 1).
  --kf           Keep the validated file after processing (default is to delete it).

FILE FORMAT:
BSSID,CHANNEL

Usage (Update channels): capture_handshakes -f bssids.txt -t 1 -i iface -u
Usage (default): capture_handshakes -f bssids.txt -t 3600 -i iface -m 1

NOTE:

-m 1 uses PineAPs currently selected interface so -i is not needed.
however it can be used with -m 1
```


## check_handshakes

This is another simple script that uses tcpdump to read a .cap/pcap file for the amount of keys it has within the capture and if a beacon or probe response is contained within it and if the associated .22000 hashcat file was successfully generated from that file.

**Usage:**
```
./check_handshakes -d handshakes
```

**Help Menu:**
```
Usage: check_handshakes -d folder | -f file

Check cleaned cap/pcap files in the specified directory for certain conditions.
Created for the WiFi Pineapple, which cleans the capture files to include only 4 Keys and 1 Beacon.
This will likely not be able to process very large captures on the pineapple.

Options:
  -d directory    Specify the directory containing cap/pcap files
  -f file         Specify a single cap/pcap file to scan

Info:
  - GREAT: Useful for both Hashcat & Aircrack-ng.
  - GOOD: Useful for Hashcat Only (Usually a Half Handshake).
  - OK: Useful for Hashcat Only (Usually a Half Handshake).
  - LIKELY BAD: Useful for Aircrack-ng Only (MUST HAVE 4 KEYS).
  - BAD: Not useful for either Hashcat or Aircrack-ng.

Conditions:
  - GREAT: 4 keys, Beacon/Probe Y, associated .22000 file Y
  - GOOD: 3 keys, Beacon/Probe Y, associated .22000 file Y
  - OK:   2 keys, Beacon/Probe Y, associated .22000 file Y
  - LIKELY BAD: 2-4 keys, Beacon/Probe Y, associated .22000 file N
  - BAD:  1-4 keys, Beacon/Probe N
  - BAD: 1 Key, Beacon/Probe Y
```

## wpa2clean

This is a script to clean a pcap/cap file and use tail and head to create a clean pcap with a single beacon and 4 keys.
This is best paired when using the pcap_split to split the file into smaller pcaps to more it easier to process and more successful.

**requires:** `tcpdump`
**install:** `opkg install tcpdump`

**Usage:**
```
./wpa2clean -f chunk.pcap -o output-clean.pcap
```

**Help Menu:**
```
Usage:
  wpa2clean -p <chunk_prefix_with_path> -o <output_file>
  wpa2clean -f <single_pcap_or_cap_file> -o <output_file>

Options:
  -p   Prefix of chunked PCAP/CAP files to merge
  -f   Single PCAP or CAP file to process
  -o   Output file for the merged data
```


## pcap_split

This C script is useful for splitting a raw capture i to smaller pcaps, allowing you to get a more precise pcap containing the first handshake pair and a beacon.
This is best paired when using the wpa2clean script and check_handshakes, to confirm the pcap has the one containing a beacon and the 4-way Handshake message pair.

**Build with:**
```
gcc -o pcap_split pcap_split.c
```

**Usage:**
```
pcap_split input.pcap chunk 1M
```

**Help Menu:**
```
Usage: pcap_split input.pcap output_prefix chunk_limit

Splits the pcap file (input.pcap) into smaller chunks. Each chunk
will start with the original pcap global header. The output files will
be named as output_prefix_000.pcap, output_prefix_001.pcap, etc. The
output_prefix may include a path (e.g., /tmp/output/myfile).

The chunk_limit should be specified in a human-readable format, e.g.:
  100B      (100 bytes)
  500K      (500 kilobytes)
  100M      (100 megabytes)
  2G        (2 gigabytes)

Example:
  pcap_split file.cap /tmp/output/myfile 100.32M
```