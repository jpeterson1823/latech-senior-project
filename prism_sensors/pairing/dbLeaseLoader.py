# imports needed to do this
from pathlib import Path
from subprocess import run  # used to run docker command
import mysql.connector      # used to connect to database

def querydb():
    # docker command used to find ip address of database server
    result_ip = run(["docker inspect -f \
    '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' mysql-compose"],
    shell=True, capture_output=True, text=True).stdout.strip()

    # create db object
    db = mysql.connector.connect(
                host=result_ip,
                user='apache-server',
                password='PRISM3',
                database='PRISM_DB'
            )
    cursor = db.cursor()

    # the sql query to send to database
    query = "SELECT * from Hardware"
    # query = "UPDATE Users SET model_path='path', audio_path='path', calendar_path='path' WHERE Username='JohnW'"

    # execute sql query
    cursor.execute(query)

    # get sql query result
    return cursor.fetchall()

if __name__ == "__main__":
    # set and create workspace
    wspath = Path(Path.home() / ".prism")
    wspath.mkdir(parents=True, exist_ok=True)
    print(f"    Workspace created at {wspath}")

    # query database for current sensor info
    sensorInfo = querydb()
    print("    Queried database successfully.")

    # open file and write leases
    with open(wspath / "module.leases", "w+") as f:
        for row in sensorInfo:
            mac = row[3]
            ip = row[4]
            print(f"{mac};{ip}")
            f.writelines(f"{mac};{ip}")
    print("    dbLeaseLoader finished!")
