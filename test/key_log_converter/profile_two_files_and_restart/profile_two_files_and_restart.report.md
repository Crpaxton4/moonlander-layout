# Key log report

## Usage

| Metric                   |   Value |
| ------------------------ | ------: |
| Files                    |       2 |
| Sessions                 |       3 |
| Logged time              | 0:00:31 |
| Key pushes               |       2 |
| Keys per minute          |     3.8 |
| Corrections per 100 keys |    0.00 |

### Top keys

| Key | Pushes |
| --- | -----: |
| A   |      1 |
| B   |      1 |

### Busiest heatmap rows

| Heatmap row    | Pushes |
| -------------- | -----: |
| `0x0004,4,0,0` |      1 |
| `0x0005,4,1,0` |      1 |

### Top shortcuts

None.

## Profiling

### Handlers

| Handler       | Calls | Average µs | Maximum µs | Share of a scan |
| ------------- | ----: | ---------: | ---------: | --------------: |
| `record_user` |    10 |     15.200 |    100.000 |          1.48 % |
| `key_log`     |     4 |      1.250 |      2.000 |          0.12 % |

### Scans and stack

| Metric                 |          Value |
| ---------------------- | -------------: |
| Core clock             |    72000000 Hz |
| Scan time              |    1023.891 µs |
| Scan rate while idle   | 1025.0 scans/s |
| Scan rate while typing |  880.0 scans/s |
| Lowest free stack      |     1500 bytes |

## Data quality

| Metric                               |   Value |
| ------------------------------------ | ------: |
| Ignored lines                        |       2 |
| Records outside sessions             |       0 |
| Handler records with no clock record |       3 |
| Time with profiling on               | 0:00:30 |
