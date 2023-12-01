import subprocess

def test_cgi(url, data):
    try:
        # Use subprocess to run the curl command
        result = subprocess.run(['curl', '-X', 'POST', '--data', data, url], capture_output=True, text=True)

        # Print the result
        print(f"Response Code: {result.returncode}")
        print("Response Body:")
        print(result.stdout)

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    # Replace 'http://localhost:8080/cgi-bin/test.cgi' with your actual CGI script URL
    cgi_url = 'http://localhost:8080/cgi-bin/test.cgi'

    # Replace 'param1=value1&param2=value2' with your actual POST data
    post_data = 'param1=value1&param2=value2'

    test_cgi(cgi_url, post_data)

