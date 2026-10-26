; Instalador de ALFREDD Gain (VST3, Windows x64) - Inno Setup 6
[Setup]
AppName=ALFREDD Gain
AppVersion=1.0.0
AppPublisher=ALFREDD
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
Uninstallable=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir=installer-output
OutputBaseFilename=ALFREDD-Gain-Setup
Compression=lzma
SolidCompression=yes

[Files]
Source: "build\ALFREDDGain_artefacts\Release\VST3\ALFREDD Gain.vst3\*"; \
    DestDir: "{commoncf64}\VST3\ALFREDD Gain.vst3"; Flags: recursesubdirs createallsubdirs ignoreversion
