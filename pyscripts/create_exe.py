import shutil,os

for i in os.listdir(os.getcwd()):
    if "EXE Dev Build" in i:
        os.remove(i)

def clear(path):
    for p in os.listdir(path):
        f = path+"\\"+p
        if os.path.isfile(f):
            os.remove(f)
        else:
            clear(f)
    os.rmdir(path)
    

dst = ".EXE"

f = open("version", 'r')
version = int(f.readline())
f.close()
f = open("version",'w')
f.write(str(version+1))
f.close()

if not os.path.exists(dst):
    os.mkdir(dst)

p = '../cmake-build-release/BURST.exe'

if os.path.exists('../cmake-build-release/BURST.exe') and os.path.exists('../cmake-build-debug/BURST.exe'):
    f1 = os.path.getmtime('../cmake-build-release/BURST.exe')
    f2 = os.path.getmtime('../cmake-build-debug/BURST.exe')

    if f2 > f1:
        p = '../cmake-build-debug/BURST.exe'

shutil.copy("../cmake-build-release/BURST.exe", dst)
for file in os.listdir("../cmake-build-release"):
    if file.endswith(".dll"):
        shutil.copy("../cmake-build-release/"+file, dst)
shutil.copytree("../assets", dst+"/assets")

epik_file_name = "BURST - Version "+str(version)+" (EXE Dev Build)"

shutil.make_archive(epik_file_name, 'zip', dst)

clear(str(os.path.abspath(dst)))
