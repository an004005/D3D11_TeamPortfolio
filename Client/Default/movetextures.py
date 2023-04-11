import subprocess
import os
import json
import shutil
import codecs
from pathlib import Path

# targetDir = '../Bin/Resources/Texture/MaterialTexture/AnimationModelTextures/'
targetDir = '../Bin/Resources/Texture/MaterialTexture/StaticModelTextures/'

def moveTexture(path, filename):
    json_data = {}
    with open(path + '/' + filename, 'r') as f:
        json_data = json.load(f)

        newTexturePath = []
        for texturePath in json_data['ShaderParams']['Textures']:
            if len(texturePath) == 0:
                newTexturePath.append("")
                continue
            absPath = str(texturePath)
            absPath = absPath.replace("\u0000", "")
            absPath = absPath.replace("\\","/")


            fileNameDDS = str(os.path.basename(absPath)).split('.')[0] + ".dds"
            outputPath = targetDir + fileNameDDS
            command = "nvtt_export.exe -o " + outputPath +  " -f 18 " + absPath
            subprocess.call(command, shell=True)

            print(command)

            newTexturePath.append(outputPath)

        json_data['ShaderParams']['Textures'] = newTexturePath
    with open(path + '/' + filename, 'w', encoding='utf-8') as make_file:
        json.dump(json_data, make_file, indent="\t")



# for (path, dir, files) in os.walk("../Bin/Resources/Materials/AnimationModel/"):
for (path, dir, files) in os.walk("../Bin/Resources/Materials/StaticModel/"):
    for filename in files:
        ext = os.path.splitext(filename)[-1]
        if ext == '.json':
            moveTexture(path, filename)
            # print("%s/%s" % (path, filename))

# subprocess.call("nvtt_export.exe -o test.dds -f 18 T_ch0100_Hair_NML.png", shell=True)