const mysql = require('mysql2');
const connection = mysql.createConnection({
  user: 'locallhost',
  password: 'drthjk1975',
  database:'jeremiaseventos',
  host: 'db.id.ap-southeast-2.rds.amazonaws.com',
  ssl: 'Amazon RDS'
});
module.exports = connection;