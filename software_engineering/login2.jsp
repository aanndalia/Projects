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
		Statement s = con.createStatement();
		String username = request.getParameter("username");
		String password = request.getParameter("password");
		String str = "SELECT password FROM testtable WHERE username='"+username+"'";
		s.executeQuery(str);
		ResultSet rs = s.getResultSet();
		while(rs.next()){
			String pass = rs.getString("password");
			if(pass.equals(password)){
				goodPass = true;
			}
			  session.setAttribute("password", pass);
			count++;
		}
		if(count > 0 && goodPass == true){
			session.setAttribute("con", con);
			response.sendRedirect("faketest.jsp");
		}
		else{
			response.sendRedirect("setlogin.html");
		}
		rs.close();
		s.close();
	}
	catch(Exception e){
		out.println("Exception: " + e.getMessage() + "<br />");
	}
	
%>
