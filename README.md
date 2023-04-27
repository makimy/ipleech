## IPLEECH

Grabber/Scanner for `ip:port` pairs inside of any file.

## Description

`ipleech` is a tool that grabs `ip:port` pairs from any file,
which will be printed or checked for every host availability.

## Install

```sh
make
sudo make install
```

## Usage

```sh
Usage: ./ipleech <filename.txt>
  -g Grab and print hosts: <default>
  -t Scan timeout: <default: 5s>
  -s Also [s]can hosts
  -h This help message
```

## Example Usage

To grab all pairs from a file use:

```sh
cat /hosts.txt | ipleech
```

To grab and scan:

```sh
cat hosts.json | ipleech -s 2> offline_hosts.txt > online_hosts.txt
```

Where `offline_hosts.txt` will contain unavailable hosts and `online_hosts.txt`
is all the hosts available.

## License

GPLv2

