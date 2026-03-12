# Rclone Basic Input Utility

A basic CLI program to facilitate a few common rclone functionalities without the need to write full commands within rclone's directory. 

## Disclaimer
- This program was made for personal use and is not associated with rclone's official software.
- The code structure is procedural and will run commands based on inputs without external changes. If you type in the wrong input, the command would either not run or will run in unintended ways. Please exercise caution when entering data. 
- The program is currently limited in its functionalities and has an inconsistent code structure.
- I'd appreciate if anyone is willing to improve this program. Thank you for spending your time exploring it! <3

## What can you do with this?
- Configure your remotes
- View your rclone program's info including list of remotes and current version.
- Sync files between remotes (both with or without output)
- Mount remote storage (currently only at full vfs cache mode)
- Terminate running rclone tasks

## More information
- Currently there's only a Windows executable of this program.
- In my tests, the ```iostream``` library has some output issues (not printing characters) when running in Command Prompt. CRLF printing is included but best performance is on Powershell.

## How to use:
1. Download the source code.
2. (Optional) Compile your desried edited C++ code 
3. Place the executable in the same directory as the rclone executable (rclone should be named as ```rclone.exe```)
4. Run the executable

## License
This project is licensed under the MIT License.

## Contribution
Feel free to contribute to this software by any way that can help with improvements of this repo. Thank you!
