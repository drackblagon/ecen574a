from os import listdir
from os import getcwd
from os.path import isfile, join
import subprocess
import glob

global_latency = "10"

source_file_folder = "/assignment3_testfiles_full/"
result_file_folder = "/results/"

exe = getcwd()  + "/xcode_project/src/Debug/hlsyn"
if not isfile(exe):
    print("Could not find exe at " + exe)
    exe = getcwd() + "/mmarsh5_hdlc_halvorsen_hlsyn/build/src/hlsyn"
    if not isfile(exe):
        raise Exception("Could not find exe file at " + exe)

source_file_path = getcwd() + source_file_folder
result_file_path = getcwd() + result_file_folder

onlyfiles = glob.glob(source_file_path + '/**/*.c', recursive=True)

for f in onlyfiles:
    if f.find(".") != 0:
        outputFileName = result_file_path + f[(f.rfind("/") + 1):f.find(".")] + ".v"
        print("Testing " + f)
#        print("Output  " + outputFileName)
        shouldFindError = False
        foundError = False
        if "error" in f:
            shouldFindError = True
        try:
            subprocess.run([exe, f, global_latency, outputFileName], check=True)
        except Exception as e:
            print("Found error at: " + f)
            foundError = True
            if not shouldFindError:
                raise(e)
        finally:
            if shouldFindError and not foundError:
                raise(Exception("Did not find error but should have"))
        print()
        print()
        print()
