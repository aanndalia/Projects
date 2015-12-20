<%@ page import="java.sql.*" %>
<html>
<body>
<%
	Connection con;
	try{
		Class.forName("com.mysql.jdbc.Driver").newInstance();
		con = DriverManager.getConnection("jdbc:mysql://localhost/testdb", "tdguest", "tdpass");
		if(!con.isClosed())
			out.println("Successfully connected to MySQL server.<br />");
		Statement s = con.createStatement();
		String str = "SELECT * FROM testtable";
		s.executeQuery(str);
		ResultSet rs = s.getResultSet();
		while(rs.next()){
			String val = rs.getString("testcol");
			out.println("testcol = " + val + "<br />");
		}
		rs.close();
		s.close();
	}
	catch(Exception e){
		out.println("Exception: " + e.getMessage() + "<br />");
	}
%>
</body>
</html>
