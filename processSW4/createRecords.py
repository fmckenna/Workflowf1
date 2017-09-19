from shutil import copyfile

inputFile='HFmeta';
lineCount=0;
with open(inputFile) as f:
    for line in f:
       if (lineCount > 0 and lineCount < 10):
           lineList=line.split(" ");
           if (lineList[0] != "S_30_20") :
               print(lineList[0])
               copyfile("S_30_20.x", lineList[0]+".x")
               copyfile("S_30_20.y", lineList[0]+".y")
               copyfile("S_30_20.z", lineList[0]+".z")
       lineCount += 1;


