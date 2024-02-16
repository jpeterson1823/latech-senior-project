# CAPSTONE PRISM WEBPAGE (MODIFIED FOR HARDWARE TESTING PURPOSES)
This branch is to aid the hardware development/testing PRISM with this web server.
Below are steps to get docker environment set up for testing purposes:
1. Have docker and docker-compose installed
2. Download the docker-compose.yml file above into your working directory
3. Pull necessary docker images for docker hub to your machine
4. Deploy docker environment using docker-compose
5. View webserver and play around

## Step 1: Install Docker and Docker-Compose
- install docker by following docker's [documentation](https://docs.docker.com/get-docker/)
- install docker compose by following docker's [documentation](https://docs.docker.com/compose/install/)

## Step 2: Download Provided Docker-Compose File
- pull this branch into your local working environment or copy the file contents into a local file

## Step 3: Pull Docker Images from Docker Hub
### Access token for Docker Hub (Read-Only)
- dckr_pat_cdNZD1At8fUeOT8l3Xm8uWg_HAc

### To Pull from Docker Hub Repo on CLI (Linux):
1. `docker login -u chh02`
2. (when prompted) enter access token from above
3. `docker pull chh02/capstone-latech-prism:mysql-image`
4. `docker pull chh02/capstone-latech-prism:apach-image`

## Step 4: Deploy Docker Environment
### Run The Following (Linux):
1. pull up your cli
2. get in the directory with docker-compose.yml
3. `docker-compose up -d`

## Step 5: View Webserver & Play
### View Running Application
- go to ['localhost'](http://localhost) in your browser

### View WebPage Designed for Demo Day
- go [here](http://localhost/pages/demo.html)

### Test GET & POST Functionality to Demo (Linux CLI)
- `curl -X GET "localhost/php/demo.php?REQ=your%20MACADDR&DATA=your%20data"`
- `curl -X POST "localhost/php/demo.php" -d "REQ=your MACADDR&DATA=your data"`

# FEEDBACK
- feel free to message me any concerns in discord

#### Thank you for your time!