from networking.addressing import MacAddr, IPv4Addr
from pairing.serialman import ModuleType

from getpass import getpass
import mysql.connector


class SensorEntry:
    def __init__(self, moduleType: ModuleType, mac: MacAddr, ip: IPv4Addr):
        self._type = moduleType
        self._mac  = mac
        self._ip   = ip
    
    def genInsertQuery(self) -> str:
        return  f"""
        INSERT INTO Hardware(type,macaddr,ipaddr)
        VALUES ({self._type},{int(self._mac.rawBytes())},{int(self._ip.rawBytes())})
        """

class Database:
    def __init__(self, host: str, user: str, pswd: str):
        self._host = host
        self._user = user
        self._pswd = pswd
        try:
            self._connection = mysql.connector.connect(
                host = host,
                user = user,
                passwd = pswd
            )
        except mysql.connector.Error as err:
            print(f'Failed to connect to MySQL database. ERROR: {err}')
            self._connection = None
    
    def query(self, query: str):
        write = query.split(' ')[0] == "INSERT"
        cursor = self._connection.cursor()
        try:
            cursor.execute(query)
            if write:
                self._connection.commit()
                return True
            else:
                return cursor.fetchAll()
        except mysql.connector.Error as err:
            print(f"Query error: {err}")