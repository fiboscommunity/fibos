# FIBOS

FIBOS is a JavaScript runtime that combines EOS and fibjs. It provides programmability to eos and allows the use of JavaScript to write smart contracts.

## Building(Ubuntu20.04、amd64/x86_64)

1. Download repository:

You need to download the current repository and obtain the code for FIBOS.

```bash
git clone https://github.com/fiboscommunity/fibos.git
```

2. Pull submodules:

Navigate to the master directory of FIBOS and use git submodule to pull the code from the submodules:

```bash
cd /path/to/fibos
# This command will ensure that all submodules are cloned and up-to-date.
git submodule update --init --recursive
```

3. Install compile dependencies:

Before compiling FIBOS, you need to install the necessary dependencies. For Ubuntu 20.04, you can use the following commands to install the required packages:

```bash
sudo apt update
sudo apt install -y ccache llvm-13 clang-13 libclang-13-dev texinfo libgmp-dev zstd pkg-config automake cmake libboost-all-dev libcurl4 libcurl4-openssl-dev libusb-1.0-0 libusb-1.0-0-dev
```

4. Compile FIBOS:

To compile FIBOS, use the build script located in the FIBOS directory. You can specify the number of CPU cores to use for the compilation process to speed it up. For example, to use 4 CPU cores, run the following command:

```bash
# This command will execute the build script and compile the FIBOS code using 4 CPU cores. Adjust the -j parameter according to the number of CPU cores available on your machine for optimal performance.
bash ./build -j4
```

5. Run FIBOS:

After successfully compiling FIBOS, you can run the program. The compiled binary file fibos will be generated in the ./bin/Linux_x64_release directory, assuming you are compiling on an Ubuntu system with an AMD64 architecture in release mode.

```bash
# 1. Navigate to the Binary Directory:
cd ./bin/Linux_x64_release
# Run the FIBOS Binary:
./fibos
```

If you want to make fibos available globally, you can copy the binary to a directory in your system's PATH, such as /usr/local/bin. This can be done using the installer.sh script provided:

```bash
sudo bash ./installer.sh
```

## Testing

To test FIBOS, follow these steps:

1. Enter the Test Directory:

Navigate to the ./test directory within the FIBOS codebase.

```bash
cd ./test
```

2. Install Required node_modules:

Use the fibos --install command to install the necessary node_modules for testing.

```bash
fibos --install
```

3. Run the Test Cases:

Execute the test cases using the fibos command.

```bash
fibos .
```

## Project Dependencies

Below is a table summarizing the project dependencies:

| Dependency                    | Version       |
|-------------------------------|-------------- |
| EOSIO(Leap)                   |  v5.0.3       |
| FIBJS                         |  v0.38.0-dev  |

## Community

* Website:    https://fibos.io
* Repository: https://github.com/fiboscommunity/fibos
* Issue:      https://github.com/fiboscommunity/fibos/issues