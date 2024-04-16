Post-build команда Eclipse:
```
mkprom2 -mcpu=v8 -freq 50 -ccprefix sparc-gaisler-elf -o pci_driver.out pci_driver ; sparc-gaisler-elf-objcopy -O srec pci_driver.out pci_driver.srec
```
Файл pci_driver.bat:
```
cd C:/Users/astsa/Downloads/graip_1.45
cmd.exe /c "graip_ahbuart.exe rf C:\Diplom\leon4sparc\pci_driver_script.txt"
CMD /K
```
Файл pci_driver_script.txt:
```
flash erase all
flash load C:\Diplom\leon4sparc\pci_driver\Debug\pci_driver.srec
ve C:\Diplom\leon4sparc\pci_driver\Debug\pci_driver.srec
run
```
