rd lib /s
del *.bak /s
del *.pdb /s
del *.map /s
del *.user /s
del *.ncb /s
del *DEBUG.exe /s
del *DEBUG.exe.manifest /s
del *.ilk /s
del *.suo /s
cd compiler
cd vc8
del *.suo
del *.xml
rd build /s
rd Vtune /s
rd _UpgradeReport_Files /s
del UpgradeLog.xml
cd ..
cd vc7
del *.xml
del *.old
del *.suo
rd build /s
rd Vtune /s
cd ..
cd ..
