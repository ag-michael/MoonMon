# Configuration

Configuration settings can be defined in one or more Yaml files.

After installation, all configuration files reside under installation directory's `Config` sub-directory.

## Configuration file structure

Yaml configuration files must always contain a top-level mapping under which there are various keys.

## Global settings

Global settings are defined under the special `global_settings` top-level mapping.

**Example**

```Yaml
global_settings:
  install_path: C:\Windows\System32\MoonMon\ # Absolute path to the desired installation directory (will be created)
  install_source_path: \\10.0.0.1\dist\MoonMon\ # Absolute path to a local or UNC path to the install directory, won't be used if http(s) installation is used
  service_name: "MM"
  service_display_name: "MoonMon Driver"
  agent_service_name: "Luna"
  agent_service_display_name: "MoonMon"
  file_output: C:\Windows\System32\MoonMon\events.log # JSON log output
  file_output_rotate_size: 200000000 # Log file size in bytes that will trigger rotation
  file_output_rotate_count: 5 # Number of log files to retain after rotation
  keep_excludes: false # When true, it will process but won't drop excluded events (for debugging)
  resist_tampering: true # turn on various tamper-resistance features
  # The rest of these settings turn on/off blocking and logging capabilities
  # For different event sources
  process_monitoring: true
  thread_monitoring: true
  module_monitoring: true
  object_monitoring: true
  registry_monitoring: true
  file_monitoring: true
  network_monitoring: true
```
## Lists and Rules

Other than `global_settings` all other top-level mappings are Lists. Lists contain entries which contain rules.
When a rule in a List matches an event monitored by MoonMon, some action will be taken depending on
the type of list in question as well as other settings defined at the Entry level.

### Lists

Here are the currently supported Lists and the actions taken when an Entry matches an event.

- `process_block_list` : Block matching events
- `process_include_list` : Log matching events
- `process_exclude_list` : Drop matching events
- `process_terminate_include_list` : Log matching events
- `process_terminate_exclude_list` : Drop matching events
- `remote_thread_include_list` : Log matching events
- `remote_thread_exclude_list` : Drop matching events
- `image_load_include_list` : Log matching events
- `image_load_exclude_list` : Drop matching events
- `object_access_block_list` : Block matching events
- `object_access_include_list` : Log matching events
- `object_access_exclude_list` : Drop matching events
- `registry_block_list` : Block matching events
- `registry_include_list` : Log matching events
- `registry_exclude_list` : Drop matching events
- `file_create_block_list` : Block matching events
- `file_create_include_list` : Log matching events
- `file_create_exclude_list` : Drop matching events
- `file_set_information_block_list` : Block matching events
- `file_set_information_include_list` : Log matching events
- `file_set_information_exclude_list` : Drop matching events
- `network_include_list` : Log matching events
- `network_exclude_list` : Drop matching events

### Entries

Fields that can be used for list entries:

- `id` : An identifier (integer)  that is globally unique (across all configuration files)
- `name` : An optional human-readable name for the entry, matching events will have this associated with them in the log outputs
- `action` : For block lists that support them, this value influences the post-block actions and status-codes that will be taken.
  - This is by default NT status codes
  - The following human-readable strings will be translated to the correct NT status codes:
    - `accessdenied`
    - `notfound`
    - `pending`
  - For `object_access_block_list` entries, the action value will be used to clear the desired access mask using an `AND` operation.
- `rules` : A list (sequence) of rules. All rules under the entry must match for entry to match and action be taken.
  - Matching "any" or a specific number of rules is planned/work-in-progress. 

### Rules

Every item in a `rules` sequence must contain the following mappings:

- `condition` : The match operator that will be used to evaluate events
- `field` : The name of the field to match
- `value` : The value that should be used to match events

### Match operators

| Match operator | Alias | Description |
| :--------------- | :------------: | :------------------------------------------------- |
| startswith | starts_with | The string starts with the value
| notstartswith | not_starts_with | The string does not start with the value
| endswith | ends_with | The string ends with the value
| notendswith | not_ends_with | The string does not end with the value
| contains | in | The string contains the value
| notcontains | not_in | The string does not contain the value
| equals | == | Exact match of the value (strings, numbers and IP addresses)
| notequals | != | Not an exact match of the value  (strings, numbers and IP addresses)
| and | & | Exact match after a logical AND
| notand | !& | Not an exact match after a logical AND operation
| greaterthan | > | The number is greater than the value
| notgreaterthan | !> | The nubmer is not greater than the value
| greaterorequal | >= | The number is greater than or equal to the value
| lessthan | < | The number is less than the value
| notlessthan | !< | The number is not less than the value
| lessorequal | <= | The number is less than or equal to the value

### Eample List

Below is a Yaml snippet containing two Lists and rules, taken from one of unit test configurations.

```Yaml
process_block_list:
    entries:
    - id: 99990000
      action: accessdenied
      name: Unit test for process_block_list
      rules:
        - condition: equals
          field: image_file_name
          value: C:\Luna_tests\PBL\test.exe
network_include_list:
    entries:
    - id: 999924000
      name: Unit test for network_include_list
      rules:
        - condition: contains
          field: command_line
          value: certutil
        - condition: equals
          field: net_remote_port
          value: 8484
        - condition: equals
          field: net_remote_ipv4_addr
          value: 192.168.56.103/25

```
