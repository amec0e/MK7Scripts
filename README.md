# MK7Scripts
Various Scripts I made with prompt engineering for the WiFi Pineapple MK7.

## DISCLAIMER:

Please keep in mind these are not entirely well crafted. There may also be potential bugs, however they perform the operations for the intended tasks!

Please **use at your own risk!**

Myself (amec0e) assumes NO liability or responsibility for any misuse or unintended consequences resulting from the use of these tools.

These tools are provided with the expectation that users will comply with all applicable laws and regulations. They are intended for educational and research purposes only.

**Please note:** Myself (amec0e) **DOES NOT** provide support for these tools.

## Contents:

1. [Macspoof](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#macspoof)
2. [Miniairo](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#miniairo)
3. [gather_probes](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#gather_probes)
4. [sort_probes](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#sort_probes)
5. [Process_MAL_Only.py](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#process_mal_onlypy)
6. [Process_MLA_Complete.py](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#process_mla_completepy)
7. [deauther](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#deauther)
8. [bpineap](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#bpineap)
9. [check_handshakes](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#check_handshakes)
10. [airedeauth](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#airedeauth)
11. [capture_handshakes](https://github.com/amec0e/MK7Scripts?tab=readme-ov-file#capture_handshakes)


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
./deauther -i wlan1mon -b AP_BSSID -d DURATION -w WAIT -r REAPEATS -p PACKET_SPEED
```

**Help Menu:**
```
Usage: ./deauther [-i INTERFACE] [-c CHAN] [-b BSSID] [-s STATION] [-t AP_BSSID_FILE] [-T STATION_BSSID_FILE] [-d DURATION] [-w WAIT] [-r REPEATS] [-p SPEED] [-h]


Options:
  -i INTERFACE    Specify the interface name to use.
  -c CHAN         Specify the channel number (overrides when using a file).
  -b BSSID        Specify the target BSSID (mandatory for single target).
  -s STATION      Specify the target STATION BSSID (mandatory for single station).
  -t FILE         Specify the path to the file containing target BSSIDs and channels (one per line, format: BSSID,CHANNEL).
  -T FILE         Specify the path to the file containing target STATION BSSIDs and channels (one per line, format: BSSID,CHANNEL).
  -d DURATION     Specify the duration of mdk4 runtime in seconds (default: 25).
  -w WAIT         Specify the delay between runs in seconds (default: 60).
  -r REPEATS      Specify how many times the cycle should repeat (default: 3).
  -p SPEED        Specify the packets per second (default: unlimited).
  -h              Show this help menu.

Default Durations:
  DURATION       30 seconds
  WAIT           40 seconds
  REPEATS        3 times
  SPEED          0 (unlimited)

Target File Format for BSSID File:

  BSSID,CHAN
  DE:AD:BE:EF:12:34,13

Target File Format for Station BSSID File (Most Effective):

  BSSID,CHAN
  00:11:22:33:44:55,10

Examples:

Single Target AP, Single Channel:
  ./deauther -i wlan1mon -c 6 -b 00:11:22:33:44:55

Single Target Station, Single Channel:
  ./deauther -i wlan1mon -s 00:11:22:33:44:55 -c 10

AP BSSID File, With Optional Defaults:
  ./deauther -i wlan3mon -t targets.txt -d 20 -w 30 -r 5

AP BSSID File, Defaults Only:
  ./deauther -i wlan1mon -t targets.txt

Station BSSID file, Defaults:
  ./deauther -i wlan1mon -T stations.txt

Station BSSID file (Quick Sweep):
  ./deauther -i wlan1mon -T stations.txt -d 11 -w 5 -r 1
```


## bpineap:

This allows you to adjust all of the options you would find using `uci show pineap` minus the `ap_interface` as this just does not work correctly due to other factors at play. This uses `uci` to set these options temporarily and then restarts pineapd to ensure the changes take affect. You can also show a scan, stop a scan and start a scan using the cli `pineap` options. It just saves time typing or copy and pasting the uci line to edit.

**Usage:**
```bash
./bpineap pineap_interface wlan1mon
```

**Help Menu:**
```
Usage: /bin/bpineap <option> <value>

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
./airedeauth -i wlan1mon -a 00:11:22:33:44:55 -s aa:bb:cc:dd:ee:ff -c 6
```

**Help Menu:**
```
Usage: ${0} [-i INTERFACE] [-a AP_BSSID] [-c CHANNEL] [-s STA_BSSID] [-t AP_BSSID_FILE] [-w WAIT] [-r REPEATS] [-n COUNT] [-x SPEED] [-R REASON] [-h]

Options:
  -i INTERFACE    Specify the interface name to use.
  -a AP_BSSID     Specify the target AP BSSID (mandatory for single target if not using -t).
  -c CHANNEL      Specify the channel number (mandatory if not using -t).
  -s STA_BSSID    Specify the target STA BSSID (mandatory for single target if not using -t).
  -t FILE         Specify the path to the file containing target AP BSSIDs and channels (one per line, format: BSSID,STATION,CHAN).
  -w WAIT         Specify the delay between runs in seconds (default: $DEFAULT_WAIT).
  -r REPEATS      Specify how many times the cycle should repeat (default: $DEFAULT_REPEATS).
  -p COUNT        Specify the number of deauthentication packet groups to send per run (default: $DEFAULT_COUNT).
  -x SPEED        Specify the packet speed (default: unlimited).
  -R REASON       Specify the deauthentication reason code (default: $DEFAULT_REASON).
  -h              Show this help menu.

Default Values:
  WAIT           $DEFAULT_WAIT seconds
  REPEATS        $DEFAULT_REPEATS times
  COUNT          $DEFAULT_COUNT packets
  SPEED          $DEFAULT_SPEED packets/second.
  REASON         $DEFAULT_REASON reason code.

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

Target File Format for AP BSSID File:

  BSSID,STATION,CHAN
  DE:AD:BE:EF:12:34,AA:BB:CC:DD:EE:FF,6

Examples:

Single Target AP, Single Channel:
  airedeauth -i wlan1mon -a 00:11:22:33:44:55 -c 6

Single Target AP and Target STA, Single Channel:
  airedeauth -i wlan1mon -a 00:11:22:33:44:55 -s aa:bb:cc:dd:ee:ff -c 6

AP BSSID File, With Optional Defaults:
  airedeauth -i wlan3mon -t targets.txt -w 30 -r 5
```


## capture_handshakes

This uses a simple loop to take a input list of BSSIDs and Channel numbers and one by one starts `pineap handshake_capture_start` and `pineap handshake_capture_stop`. This allows you to start a dedicated handshake capture the same way you would if using the WebUI, to simply stop and start capturing handshakes for different BSSIDs on different channels. Use this with `screen` and you can start a 24 hour handshake capture spree targeting selected access points on their corresponding channels. You can only run one instance of this as it will conflict with other scans otherwise and there might be some unintended behaviour.

**Usage:**
```
./capture_handshakes -i file.txt -t 60 -s
```

**Help Menu:**
```
Usage: $0 [-h] [-i FILE] [-t TIMER] [-s]
Options:
  -h          Display this help menu."
  -i FILE     Input file containing BSSIDs and channels.
  -t TIMER    Time to capture per target in seconds.
  -s          Shuffle lines in input file (Switch).
  
Input File Format:
  BSSID1,CHANNEL"
  BSSID2,CHANNEL"
  BSSID3,CHANNEL"

Input file must not contain spaces.
```


## check_handshakes

This is another simple script that uses tcpdump to read a .cap/pcap file for the amount of keys it has within the capture and if a beacon or probe response is contained within it and if the associated .22000 hashcat file was successfully generated from that file.

**Usage:**
```
./check_handshakes -d handshakes
```

**Help Menu:**
```
Usage: ./check_handshakes -d DIR

Check cleaned cap/pcap files in the specified directory for certain conditions."
Created for the WiFi Pineapple, which cleans the capture files to include only 4 Keys and 1 Beacon.
This will NOT WORK as intended with raw captures.

Options:
  -d directory    Specify the directory containing cap/pcap files

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
