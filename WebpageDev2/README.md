# CAPSTONE PRISM WEBPAGE (UNDER DEV.)
This branch is for the development of PRISM's webpage. It uses docker compose, runs on an apache server, and connects to a mysql server for its database. The webpage itself is make with html, css, and php. Below describes:
- how to preview the webpage
- access my docker hub repo
- pull resourcees from my repo
- how to deploy apache and mysql servers using docker compose
- and how to view the deployed webpage 

## View Demo of Website Here (slightly outdated)
- Click for: [Demo Website](https://chh02.github.io/CapstoneWebsite/index.html)

## Access token for Docker Hub (Read-Only)
- dckr_pat_cdNZD1At8fUeOT8l3Xm8uWg_HAc

## Docker Hub Images
### MySQL Server Image
- chh02/capstone-latech-prism:mysql-image
### Apache2 Server Image
- chh02/capstone-latech-prism:apach-image

## To Pull from Docker Hub Repo on CLI (Linux):
1. `docker login -u chh02`
2. (when prompted) enter access token from above
3. `docker pull chh02/capstone-latech-prism:mysql-Ver6`
4. `docker pull chh02/capstone-latech-prism:apach-Ver3`

## To Run Entire Application (Linux):
1. pull up your cli
2. get in the directory with docker-compose.yml
3. `docker-compose up -d`

### View Running Application
- go to ['localhost'](http://localhost:40553) in your browser

# TO TEST DATABASE
This section is to describe how to currently test and view the web servers interaction with the database.
### Step 1
- deploy application as described in above sections
- go to ['localhost'](http://localhost:40553) to view application

### Step 2
- navigate to [Database Testing](http://localhost:40553/third.php) link
- here you should be able to see a text box and two buttons (submit & reset)

### Step 3
- play around
- reset button resets the database (`TRUNCATE TABLE XXX`)
- submit button submits anything entered into the text box to the database (`INSERT INTO XXX (column1) VALUES (value1)`)
- the text displayed underneath displays values in the database in table XXX (i.e. Name: ...)
- the text displayed underneath also displays some debugging text between the php (w/ mysqli extension) and the database (MySQL)

### Step 4
- provide feedback

#### Thank you for your time!