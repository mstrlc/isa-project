import sys
import subprocess

# Get port from arguments
if len(sys.argv) > 1:
    host = sys.argv[1]
    port = sys.argv[2]

passed = 0
failed = 0

# Open file
with open('testing.txt', 'r') as f:
    for line in f.readlines():
        if(line.startswith('--')):
            print(line)
        if(line.startswith('(')):
            filter = line.split('#')[0].strip()
            expected = line.split('#')[1].strip()
            print("Filter: " + filter)
            print("Result: " + expected)
            command = "ldapsearch -v -h " + host + " -p " + port + " -x \"" + filter + "\""
            print(command)
            run = subprocess.run(command, shell=True, capture_output=True)
            output = run.stdout.decode('utf-8').split('\n')
            for line in output:
                if("numResponses" in line):
                    responses = int(line.split(':')[1])-1
                    expected_responses = int(expected)
                    break
            if(responses == expected_responses):
                print("Test passed!")
                passed += 1
            else:
                passed = False
                print("\033[91mTest failed!\033[0m")
                print("Expected: " + str(expected))
                print("Got: " + str(responses))
                failed += 1

if(failed == 0):
    print("All tests passed!")
    print("Passed: " + str(passed))
else:
    print("Some tests failed!")
    print("Failed: " + str(failed))
    sys.exit(1)