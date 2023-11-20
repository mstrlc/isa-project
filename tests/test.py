import sys
import subprocess

# Get port from arguments
if len(sys.argv) > 1:
    host = sys.argv[1]
    port = sys.argv[2]

passed = True

# Open file
with open('testing.txt', 'r') as f:
    for line in f.readlines():
        if(line.startswith('--')):
            print(line)
        if(line.startswith('"')):
            filter = line.split('#')[0].strip()
            expected = line.split('#')[1].strip()
            print("Filter: " + filter)
            print("Result: " + expected)
            command = "ldapsearch -v -h " + host + " -p " + port + " -x " + filter
            print(command)
            run = subprocess.run(command, shell=True, capture_output=True)
            output = run.stdout.decode('utf-8')
            for line in output:
                if(line.startswith('# numReponses')):
                    responses = int(line.split(':')[1])
                    expected_responses = int(expected+1)
                    break
            if(responses == expected_responses):
                print("Test passed!")
            else:
                passed = False
                print("Test failed!")
                print("Expected: " + str(expected_responses))
                print("Got: " + str(responses))

if(passed):
    print("All tests passed!")
else:
    print("Some tests failed!")
    sys.exit(1)