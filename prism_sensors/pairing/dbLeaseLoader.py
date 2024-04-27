# imports needed to do this
from subprocess import run  # used to run docker command
import mysql.connector      # used to connect to database

def querydb():
    # docker command used to find ip address of database server
    result_ip = run(["docker inspect -f \
    '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' mysql-compose"],
    shell=True, capture_output=True, text=True).stdout.strip()

    print(f"'{result_ip}'")

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
    with open("/tmp/module.leases", "w+") as f:
        for ip,mac in querydb():
            f.writelines(f"{mac};{ip}")
