### 1. Board

#### 1.1 Board Info

Board Name: Raspberry Pi 4B.

GPIO Pin: INT GPIO17.

### 2. Install

#### 2.1 Dependencies

Install the necessary dependencies.

```shell
sudo apt-get install libgpiod-dev pkg-config cmake -y
```

#### 2.2 Makefile

Build the project.

```shell
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

#### 2.3 CMake

Build the project.

```shell
mkdir build && cd build 
cmake .. 
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

Test the project and this is optional.

```shell
make test
```

Find the compiled library in CMake. 

```cmake
find_package(ir_remote REQUIRED)
```

### 3. IR_REMOTE

#### 3.1 Command Instruction

1. Show ir_remote chip and driver information.

    ```shell
    ir_remote (-i | --information)  
    ```

2. Show ir_remote help.

    ```shell
    ir_remote (-h | --help)        
    ```

3. Show ir_remote pin connections of the current board.

    ```shell
    ir_remote (-p | --port)              
    ```

4. Run ir_remote read test, num means test times.

    ```shell
    ir_remote (-t read | --test=read) [--times=<num>]
    ```

5. Run ir_remote read function, num means read times.

    ```shell
    ir_remote (-e read | --example=read) [--times=<num>]   
    ```

#### 3.2 Command Example

```shell
./ir_remote -i

ir_remote: chip is NEC IR REMOTE.
ir_remote: manufacturer is NEC.
ir_remote: interface is GPIO.
ir_remote: driver version is 1.0.
ir_remote: min supply voltage is 2.7V.
ir_remote: max supply voltage is 5.5V.
ir_remote: max current is 1.50mA.
ir_remote: max temperature is 125.0C.
ir_remote: min temperature is -40.0C.
```

```shell
./ir_remote -p

ir_remote: INT connected to GPIO17(BCM).
```

```shell
./ir_remote -t read --times=3

ir_remote: chip is NEC IR REMOTE.
ir_remote: manufacturer is NEC.
ir_remote: interface is GPIO.
ir_remote: driver version is 1.0.
ir_remote: min supply voltage is 2.7V.
ir_remote: max supply voltage is 5.5V.
ir_remote: max current is 1.50mA.
ir_remote: max temperature is 125.0C.
ir_remote: min temperature is -40.0C.
ir_remote: start receive test.
ir_remote: irq ok.
ir_remote: add is 0x00 and cmd is 0x16.
ir_remote: irq repeat.
ir_remote: add is 0x00 and cmd is 0x16.
ir_remote: irq ok.
ir_remote: add is 0x00 and cmd is 0x19.
ir_remote: irq repeat.
ir_remote: add is 0x00 and cmd is 0x19.
ir_remote: irq ok.
ir_remote: add is 0x00 and cmd is 0x5E.
ir_remote: finish receive test.
```

```shell
./ir_remote -e read --times=3

1/3.
ir_remote: irq ok.
ir_remote: add is 0x00 and cmd is 0x18.
2/3.
ir_remote: irq ok.
ir_remote: add is 0x00 and cmd is 0x5E.
3/3.
ir_remote: irq ok.
ir_remote: add is 0x00 and cmd is 0x1C.
```

```shell
./ir_remote -h

Usage:
  ir_remote (-i | --information)
  ir_remote (-h | --help)
  ir_remote (-p | --port)
  ir_remote (-t read | --test=read) [--times=<num>]
  ir_remote (-e read | --example=read) [--times=<num>]

Options:
  -e <read>, --example=<read>    Run the driver example.
  -h, --help                     Show the help.
  -i, --information              Show the chip information.
  -p, --port                     Display the pin connections of the current board.
  -t <read>, --test=<read>       Run the driver test.
      --times=<num>              Set the running times.([default: 3])
```
