# :new_moon: MoonMon :new_moon:

MoonMon is an endpoint monitoring and policy enforcement tool for Microsoft Windows.

# What does it do?

MoonMon is intended to be an endpoint security tool that enables rule-centric detailed monitoring and restrictions on a Windows system.

MoonMon is inspired by tools like Sysmon and Linux kernel security modules. It focuses on exposing as much telemetry as possible to its user while at the same time allowing lots of configuration and fine-tuning to fit its intended deployment's use-case. 

MoonMon also exposes preventive capabilities, allowing users to restrict what operations are allowed or not allowed on their device using the same telemetry information that it collects for monitoring. 

MoonMon is not an EDR (Endpoint Detection and Response) or an Anti-Virus tool. Despite having features somewhat similar to EDRs, it is not and will never be a full-fledged comprehensive endpont detection and response tool.
Commercial EDRs not only have more capabilities, they also have curated content and up-to-date threat intelligence which they use to protect endpoints. MoonMon on the otherhand does not detect or prevent threats, unless users craft specific rules to do so.


## The big and long list of features and ideas!

The table below lists features that are currently supported,implemented, planned or are being researched.

### Status legend
* :heavy_check_mark: Supported
* :100: Implemented and stable but unsupported (likely due to lack of testing)
* :x: Not supported or implemented but planned
* :warning: Partially implemented and/or work-in-progress, likely buggy and unstable
* :bulb: Just an idea being researched at this point, part of the roadmap but no solid plans to implement it

### Features

| Feature     | Status      | Comments |
| ----------- | --------------------------- | ------------------- |
| File output logging      | :heavy_check_mark: | JSON logging supported  |
| Log rotation   | :100:| Seems to work, lacks tests |
| Install/Uninstall | :heavy_check_mark: | |
| Remote binary logging with mTLS | :x: | |
| Process creation logging | :heavy_check_mark: | |
| Process creation blocking | :heavy_check_mark: | |
| Process termination logging | :heavy_check_mark: | |
| Remote thread creation logging | :heavy_check_mark: | |
| Remote thread creation blocking | :x: | This will probably just be process termination |
| Module loading (DLLs and Drivers) logging | :heavy_check_mark: | |
| Module loading (DLLs and Drivers) blocking | :x: | This will likely mean dynamic file create blocking |
| Process Access logging | :heavy_check_mark: | |
| Process Access blocking | :100: | Unit tests passing, but it needs more extensive testing |
| Registry event logging | :warning: | All planned registry events implemented, but with limited unit tests; various registry event types haven't been tested at all |
| Registry event blocking | :warning: | All planned registry events implemented, but with limited unit tests; various registry event types haven't been tested at all  |
| File create logging | :heavy_check_mark: | This includes File open and create events. Works as expected and basic tests are passing, but it needs more extensive testing |
| File create blocking | :100: |  This includes File open and create events.Works as expected and basic tests are passing, but it needs more extensive testing |
| File set information logging | :warning: | This includes file deletion, renaming, timestamp updates and much more. Works as expected and basic tests are passing, but it needs more extensive testing |
| File set information blocking | :warning: | This includes file deletion, renaming, timestamp updates and much more. Works as expected and basic tests are passing, but it needs more extensive testing |
| Network/WFP IPV4 logging | :100: | Needs more extensive testing |
| Network/WFP IPV6 logging | :100: | Needs more extensive testing |
| Network/WFP resource assignment logging | :warning: | Untested |
| Network/WFP DNS cache logging | :x: | |
| Tamper resistance | :warning: | Some user-space agent protections against process termination exist. Overall incomplete. Registry and file tampering resistance does not yet exist. |
| Hash measurement | :x: | This is rather simple, but doing it without crashing the systems' performance requires careful planning - all from user-space |
| Hash blocking | :x: | This will likely be for process creation and module loading, but it will be asynchronous (terminate after the fact) |
| PE header enrichment | :x: | User-space/agent |
| File magic enrichment | :x: | User-space/agent |
| KAPC injection rules | :x: | Rules that will inject specific code into specific processes |
| Yara scanning | :bulb: | Rules that will define which Yara rules should be used to scan which files or process memory (user-space) |
| Safe & Dynamic reloading | :x: | |
| Configuration fallback/revert | :x: | Last known good configuration fallback when health-checks fail |
| Configuration signature enforcement | :bulb: | |
| AMSI scanning | :bulb: | |
| ETW logging | :bulb: | |
| ADS tagging | :bulb: | This seems simple, and it sort of is, but it will be an extremely powerful feature! |
| Automatic updates | :x: | |
| DLL hooking for monitoring & blocking | :bulb: | Microsoft detours? |

## High-level Roadmap


- [ ] Version 1.0
    - [ ] Microsoft Driver security checklist
    - [x] Microsoft Driver Minimum Rules - Code analysis passing
    - [x] Basic event logging
    - [x] File output and log rotation
    - [x] Basic unit-tests written and passing
    - [X] Complete docs for supported features
    - [ ] Passing unit tests on different Windows 11 installation environments
    - [ ] Decent sample config catalog
    - [ ] :grey_exclamation: Independent security reviews? (:eyes: Help Needed)
- [ ] Version 2.0
  - [ ] Signed Driver ( :heavy_dollar_sign::heavy_dollar_sign::heavy_dollar_sign::heavy_dollar_sign: ), ready for general & production use
  - [ ] Signed user-space agent
  - [ ] Performance benchmarking and tuning
  - [ ] Remote binary logging with mTLS
  - [ ] Yara scanning rules
  - [ ] Hash measurement
  - [ ] KAPC injection rules
  - [ ] PE header enrichment
  - [ ] DNS logging
  - [ ] Explore and support additional WFP layers
  - [ ] Module blocking
  - [ ] Thread creation blocking (??)
  - [ ] Safe & Dynamic reloading
  - [ ] Last known good configuration fallback when health-checks fail
  - [ ] Configuration signature enforcement
  - [ ] Automatic remote configuration and installation update
  - [ ] Filesystem and Registry tamper-resistant features (complete tamper resistance)
  - [ ] Extensive unit tests, including for tamper resistance
- [ ] Version X.X
  - [ ] ETW monitoring and prevention
  - [ ] AMSI scanning (using Yara)
  - [ ] Fuzz testing!
  - [ ] NTFS ADS tagging rules
  - [ ] Configuration convererters for Sigma, Sysmon and other formats
- [ ] Some day in the distant future
  - [ ] ELAM driver & WHQL Certification :heart_eyes: (beg Microsoft! :pray: :heavy_dollar_sign::heavy_dollar_sign::heavy_dollar_sign: )
  - [ ] Re-write everything in Rust (why not! everyone does it!)
  - [ ] DLL hooking for monitoring & blocking (works using KAPC injection)
  - [ ] Complex rule logic with a state-machine to match related events
        
# Test signing mode
For development releases, test signing must be enabled.

To enable test signing mode, run the following as administrator and reboot the system:

`bcdedit /set TESTSIGNING ON`

# Quick install

The quickest way to install MoonMon is using the following powershell command:

```powershell
powershell.exe -c 'Invoke-WebRequest -outfile MoonMon.zip -uri https://github.com/ag-michael/MoonMon/releases/download/development/MoonMon-alpha.zip;expand-archive -path MoonMon.zip -destinationpath c:\Windows\Temp\MoonMon;C:\Windows\temp\MoonMon\luna.exe install C:\Windows\Temp\MoonMon\config\luna.yaml'
```

# Docs
See [Documentation](/docs/Readme.md)

# :skull: :rotating_light: :warning: Project Status :warning: :rotating_light: :skull:

MoonMon is currently in its early development stage. While most of its advertised features work as intended, it is not ready be used in production.

This means..

*  It cannot run without test signing mode being enabled (This is by design. release versions will work without test signing)
   * This might require disabling secure boot
*  It should not be operated on systems handling data that is sensitive to loss of any kind
*  There are likely several bugs, including undiscovered security vulnerabilities
*  The currently intended audience is:
   * Developers who want to contribute to MoonMon
   * Alpha & Beta testers who are interested in testing out MoonMon and reporting issues and feature requests

Currently, MoonMon is supported only on Windows 11 installations on the x64 architecture. 


# Contributions

Much help is needed with testing MoonMon and improving it.
Pull-Requests are welcome, as are issues and feature requests.

Issues should contain enough information to reproduce the problem.
