# mod-persistent-scale

A lightweight AzerothCore module that provides **persistent per-character player scale**.

Character scale is stored in the characters database and automatically reapplied when the character logs in, changes shapeshift form, or changes display/model.

## Features

- Persistent scale stored per character
- Scale survives logout/login
- Scale survives Druid shapeshifting
- Scale survives display/model changes
- Works with custom morph and flight forms
- Uses AzerothCore's existing `SetObjectScale()` functionality
- No AzerothCore core-file modifications required
- Database table is created automatically
- Supports scale values from `0.1` to `10.0`

## Installation

Copy the `mod-persistent-scale` folder into the server's `modules` directory:

```text
modules/
└── mod-persistent-scale/
    ├── CMakeLists.txt
    ├── README.md
    └── src/
        └── persistent_scale.cpp
```

Rebuild the AzerothCore worldserver after installing the module.

Start the worldserver normally. The required database table will be created automatically in the **characters database**.

## Usage

Set the persistent scale for the currently logged-in character:

```text
.persistscale <scale>
```

Example:

```text
.persistscale 1.2
```

This sets the character to 120% of normal size and stores the setting permanently.

### Reset to normal size

```text
.persistscale 1
```

### Examples

```text
.persistscale 0.8
```

Sets the character to 80% normal size.

```text
.persistscale 1.5
```

Sets the character to 150% normal size.

```text
.persistscale 2
```

Sets the character to twice normal size.

## Scale Limits

The module accepts values between:

```text
0.1 – 10.0
```

Values outside this range are rejected.

## Database

The module automatically creates the following table in the characters database:

```sql
persistent_character_scale
```

The table stores:

- `guid` — Character GUID
- `scale` — Persistent character scale

No manual SQL installation is required.

## Compatibility

Designed for **AzerothCore WotLK 3.3.5a**.

The module uses AzerothCore scripting hooks rather than modifying core source files.

## Notes

The module reapplies the saved scale during:

- Character loading/login
- Shapeshifting
- Display/model changes

This allows persistent scaling to remain in effect when using Druid forms and other systems that change the player's display ID.

## Command Permission

The `.persistscale` command uses AzerothCore's existing `RBAC_PERM_COMMAND_MODIFY_SCALE` permission.

Users who can use the normal `.modify scale` command should therefore have the required permission.

## Credits

Created as a standalone AzerothCore module for persistent per-character player scaling.
