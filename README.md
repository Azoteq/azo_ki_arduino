# Introduction

Developed for IQS7220A, IQS7320A, and IQS9320 products from Azoteq.

Target Hardware : RP2040




# Serial Frame Composition

| Position  | Value |
| -         | -     |
| 0         | SOF 1 |
| 1         | SOF 2 |
| 2         | Frame Length |
| 3         | Frame ID |
| 4         | Command |
| ...       | Command Parameters |
| N-3       | CRC16 LSB |
| N-2       | CRC16 MSB |
| N-1       | EOF 1 |
| N         | EOF 2 |

# Commands List

## Generic Commands
| Value | Name | Description | Parameters |
| - | - | - | - |
| 0x00 | Device Setup   | Select device and matrix size | - |
| 0x01 | Stop Streaming | Stop all streaming | - |
| 0x02 | Stop Serial Comms | Stop all streaming | - |

## IQS7220A
| Value | Name | Description | Parameters |
| - | - | - | - |
| 0x10 | Key Scan | Get device and channel data of all <br> devices in the matrix <br> Return 1 byte per device | - |
| 0x11 | I2C Read Single Device | Return I2C data | 0 - Device Select <br> 1 - Device Address <br> 2 - Register Address <br> 3 - Data Length |
| 0x12 | I2C Write Single Device | Standard return | 0 - Device Select <br> 1 - Device Address <br> 2 - Register Address <br> 3 - Data Length <br> 4 - Data[] |
| 0x13 | I2C Read Multiple Devices | Return I2C data | 0 - Device Address <br> 1 - Register Address <br> 2 - Data Length |
| 0x14 | I2C Write Multiple Devices | Standard return | 0 - Device Address <br> 1 - Register Address <br> 2 - Data Length <br> 3 - Data[] |
| 0x15 | Stream Key Scan | Periodically return device and channel states | 0 - Sample Interval |
| 0x16 | Stream I2C Read Single Device | Periodically return I2C data | 0 - Sample Interval <br> 1 - Device Select <br> 2 - Device Address <br> 3 - Number of Registers <br> 4 - Register Address[] <br> 5 - Data Length[] |
| 0x17 | Stream I2C Read Multiple Devices | Periodically return I2C data | 0 - Sample Interval <br> 1 - Device Address <br> 2 - Number of Registers <br> 3 - Register Address[] <br> 4 - Data Length[] |

## IQS7320A
| Value | Name | Description | Parameters |
| - | - | - | - |
| 0x20 | Key Scan | Get device and channel data of all <br> devices in the matrix <br> Return 1 byte per device | - |
| 0x21 | I2C Read Single Device | Return I2C data | 0 - Device Select <br> 1 - Device Address <br> 2 - Register Address <br> 3 - Data Length |
| 0x22 | I2C Write Single Device | Standard return | 0 - Device Select <br> 1 - Device Address <br> 2 - Register Address <br> 3 - Data Length <br> 4 - Data[] |
| 0x23 | I2C Read Multiple Devices | Return I2C data | 0 - Device Address <br> 1 - Register Address <br> 2 - Data Length |
| 0x24 | I2C Write Multiple Devices | Standard return | 0 - Device Address <br> 1 - Register Address <br> 2 - Data Length <br> 3 - Data[] |
| 0x25 | Autonomous Mode Selection | Enter or exit autonomous mode <br> 1 - Exit <br> 2 - Enter | 0 - Selection |
| 0x26 | Standby Mode Selection | Enter or exit standby mode <br> 1 - Exit <br> 2 - Enter | 0 - Selection |
| 0x27 | Stream Key Scan | Periodically return device and channel states | 0 - Sample Interval |
| 0x28 | Stream I2C Read Single Device | Periodically return I2C data | 0 - Sample Interval <br> 1 - Device Select <br> 2 - Device Address <br> 3 - Number of Registers <br> 4 - Register Address[] <br> 5 - Data Length[] |
| 0x29 | Stream I2C Read Multiple Devices | Periodically return I2C data | 0 - Sample Interval <br> 1 - Device Address <br> 2 - Number of Registers <br> 3 - Register Address[] <br> 4 - Data Length[] |

## IQS9320 I2C
| Value | Name | Description | Parameters |
| - | - | - | - |
| 0x30 | I2C Read Single Device | Return I2C data |0 - Device Address <br> 1 - Register Address LSB <br> 2 - Register Address MSB <br> 3 - Data Length |
| 0x31 | I2C Write Single Device | Standard return | 0 - Device Address <br> 1 - Register Address LSB <br> 2 - Register Address MSB <br> 3 - Data Length <br> 4 - Data[] |
| 0x32 | I2C Read Multiple Devices | Return I2C data |0 - Number of Devices <br> 1 - Device Address[] <br> 2 - Register Address LSB <br> 3 - Register Address MSB <br> 4 - Data Length |
| 0x33 | I2C Write Multiple Devices | Standard return | 0 - Number of Devices <br> 1 - Device Address[] <br> 2 - Register Address LSB <br> 3 - Register Address MSB <br> 4 - Data Length <br> 5 - Data[] |
| 0x34 | Stream I2C Read Single Device | Periodically return I2C data | 0 - Sample Interval <br> 1 - Device Address <br> 2 - Number of Registers <br> 3 - Register Address[] <br> 4 - Data Length[] |
| 0x35 | Stream I2C Read Multiple Devices | Periodically return I2C data | 0 - Sample Interval <br> 1 - Number of Devices <br> 2 - Device Address[] <br> 3 - Number of Registers <br> 4 - Register Address[] <br> 5 - Data Length[] |

## IQS9320 Key Scan
| Value | Name | Description | Parameters |
| - | - | - | - |
| 0x40 | Key Scan | Get device and channel data of all <br> devices in the matrix <br> Return 3 byte per device | 0 - Number of Channels |
| 0x41 | I2C Read Single Device | Return I2C data | 0 - Device Select <br> 1 - Device Address <br> 2 - Register Address LSB <br> 3 - Register Address MSB <br> 4 - Data Length |
| 0x42 | I2C Write Single Device | Standard return | 0 - Device Select <br> 1 - Device Address <br> 2 - Register Address LSB <br> 3 - Register Address MSB <br> 4 - Data Length <br> 5 - Data[] |
| 0x43 | I2C Read Multiple Devices | Return I2C data | 0 - Device Address <br> 1 - Register Address LSB <br> 2 - Register Address MSB <br> 3 - Data Length |
| 0x44 | I2C Write Multiple Devices | Standard return | 0 - Device Address <br> 1 - Register Address LSB <br> 2 - Register Address MSB <br> 3 - Data Length <br> 4 - Data[] |
| 0x45 | Standby Mode Selection | Enter or exit standby mode <br> 1 - Exit <br> 2 - Enter | 0 - Selection |
| 0x46 | Stream Key Scan | Periodically return device and channel states | 0 - Sample Interval <br> 1 - Number of Channels |
| 0x47 | Stream I2C Read Single Device | Periodically return I2C data | 0 - Sample Interval <br> 1 - Device Select <br> 2 - Device Address <br> 3 - Number of Registers <br> 4 - Register Address[] <br> 5 - Data Length[] |
| 0x48 | Stream I2C Read Multiple Devices | Periodically return I2C data | 0 - Sample Interval <br> 1 - Device Address <br> 2 - Number of Registers <br> 3 - Register Address[] <br> 4 - Data Length[] |



