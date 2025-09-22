# Installation

## Quick install
The quickest way to install MoonMon is using the following powershell command:

`powershell.exe -c 'Invoke-WebRequest -outfile MoonMon.zip -uri https://github.com/ag-michael/MoonMon/releases/download/development/MoonMon-alpha.zip;expand-archive -path MoonMon.zip -destinationpath c:\Windows\Temp\MoonMon;C:\Windows\temp\MoonMon\luna.exe install C:\Windows\Temp\MoonMon\config\luna.yaml'`
## Installation options

Configuration options are detailed under [Configuration](/docs/Configuration.md).

The releas archives come with a copy of `luna.exe` which handles installation. The simplest way of installing a specific release is using an http(s) URL that points to a pre-packaged release zip-archive:

`luna.exe install https://github.com/ag-michael/MoonMon/releases/download/development/MoonMon-alpha.zip`

If an http(s) URL can't be used, a locally-accessible directory,or a UNC path to one can be used. This directory must contain all the same files that come with the release archive, but the configuration files under the `Config` directory can be customized for specific deployments.

Local installation examples:

`luna.exe install c:\temp\MoonMon`

`luna.exe install \\10.0.0.1\dist\MoonMon`

The installation source directory must contain a `Config` sub-directory that in turn contains a `luna.yaml` configuration file,
which dictates how MoonMon will be installed.

For troubleshooting or other purposes, the Luna agent can be executed directly (instead of as a service).
The comand-line for this is the same as its command-line when running as a service:

`luna.exe MoonMon`

Where `MoonMon` is the name of the kernel-mode driver's service.

# Uninstallation

Uninstallting MoonMon is a simple as:

`luna.exe uninstall MoonMon`

Where `MoonMon` is the name of the kernel-mode driver's service.

Upon removal of the driver and agent services, it will ask if the user wants to reboot the device. 
This is important because especially when tamper-resistance features are turned on, it won't be fully removed without a reboot.

As part of tamper-resistance, uninstall protection is WIP (work-in-Progress).

