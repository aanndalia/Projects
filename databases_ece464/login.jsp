<%@ page import="java.sql.*" %>
<%
	int count = 0;
	Connection con;
	try{
		Class.forName("com.mysql.jdbc.Driver").newInstance();
		con = DriverManager.getConnection("jdbc:mysql://localhost/OfficeDB", "default", "madoffice");
		if(!con.isClosed())
			out.println("Successfully connected to MySQL server.<br />");
		Statement s = con.createStatement();
		String username = request.getParameter("username");
		String password = request.getParameter("password");
		String str = "SELECT ename, gname FROM Employees WHERE username='"+username+"' AND password='"+password+"'";
		s.executeQuery(str);
		ResultSet rs = s.getResultSet();
		while(rs.next()){
			String ename = rs.getString("ename");
			String gname = rs.getString("gname");
			//out.println("testcol = " + val + "<br />");
			session.setAttribute("ename", ename);
			session.setAttribute("gname", gname);
			count++;
		}
		if(count > 0){
			session.setAttribute("con", con);
			if(((String)session.getAttribute("gname")).equals("Manager")){
				response.sendRedirect("manager.jsp");
			}
			else{
				response.sendRedirect("regular.jsp");
			}
		}
		else{
			response.sendRedirect("madawesomehtmllogin.html");
		}
		rs.close();
		s.close();
	}
	catch(Exception e){
		out.println("Exception: " + e.getMessage() + "<br />");
	}
	
%>
