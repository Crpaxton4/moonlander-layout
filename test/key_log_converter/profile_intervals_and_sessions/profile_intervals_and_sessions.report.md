# Key log report

## Usage

| Metric                   |   Value |
| ------------------------ | ------: |
| Files                    |       1 |
| Sessions                 |       2 |
| Logged time              | 0:00:18 |
| Key pushes               |       3 |
| Keys per minute          |    10.0 |
| Corrections per 100 keys |    0.00 |

### Top keys

| Key | Pushes |
| --- | -----: |
| A   |      2 |
| B   |      1 |

### Busiest heatmap rows

| Heatmap row    | Pushes |
| -------------- | -----: |
| `0x0004,4,0,0` |      2 |
| `0x0005,4,1,0` |      1 |

### Top shortcuts

None.

## Profiling

### Handlers

| Handler       | Calls | Average µs | Maximum µs | Share of a scan |
| ------------- | ----: | ---------: | ---------: | --------------: |
| `record_user` |     5 |     20.778 |     50.000 |          2.13 % |
| `key_log`     |     5 |      2.600 |      5.000 |          0.27 % |

### Scans and stack

| Metric                 |          Value |
| ---------------------- | -------------: |
| Core clock             |    72000000 Hz |
| Scan time              |     975.610 µs |
| Scan rate while idle   | 1200.0 scans/s |
| Scan rate while typing |  966.7 scans/s |
| Lowest free stack      |     2000 bytes |

## Data quality

| Metric                               |   Value |
| ------------------------------------ | ------: |
| Ignored lines                        |       3 |
| Records outside sessions             |       2 |
| Handler records with no clock record |       0 |
| Time with profiling on               | 0:00:20 |
