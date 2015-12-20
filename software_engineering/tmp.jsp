<%@ page import="java.sql.*" %>
<%
	int count = 0;
	Connection con;
	boolean goodPass = false;
	try{
		Class.forName("com.mysql.jdbc.Driver").newInstance();
		con = DriverManager.getConnection("jdbc:mysql://localhost/setdb", "root", "password");
		if(!con.isClosed())
			out.println("Successfully connected to MySQL server.<br />");
	}
	catch(Exception e){
		out.println("Exception: " + e.getMessage() + "<br />");
	}
	
%>
