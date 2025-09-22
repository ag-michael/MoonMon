# How MoonMon works

## Agent and Sensors

MoonMon uses a sensor-agent arechitecture.

### Sensors

Senors are telemetry sources that collect events for logging and take preventive action.

Currently, MoonMon has only one sensor: The Kernel-mode driver that uses various callbacks and filters.

In the future, additional sensors might be added such as ETW telmetry, AMSI scanning, APC injected hooks, network traffic capture and more.

### The agent

MoonMon uses a user-space windows service, Luna, to configure sensors and collect events.
Depending on configuraton settings, it also handles log formatting, processing, enrichment and forwarding to a remote server.

Luna also handles the installation, uninstallation of the entire application as well as dynamic configuration and application updates (Not implemented).
