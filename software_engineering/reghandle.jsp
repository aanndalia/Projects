<%@ page import="java.sql.*" %>
<%
Connection con = (Connection)session.getAttribute("con");
	
	String username = request.getParameter("username");
	String password1 = request.getParameter("password1");
	String password2 = request.getParameter("password2");

	session.setAttribute("reg_username", username);
	
	try{
		Class.forName("com.mysql.jdbc.Driver").newInstance();
		con = DriverManager.getConnection("jdbc:mysql://199.98.20.104/setdb", "guest2", "password");
		if(!con.isClosed())
			out.println("Successfully connected to MySQL server.<br />");
		Statement s = con.createStatement();
			boolean legal = true;
			String message = "";
			
			String query1 = "SELECT * FROM testtable WHERE username='"+username+"'";
			s.executeQuery(query1);
			ResultSet rs2 = s.getResultSet();
			if(rs2.next()){
				legal = false;
				message += "Error: Username already exists<br />";
			}
			if(password1.equals(password2) == false){
				legal = false;
				message += "Error: Passwords do not match<br />";
			}
			if(username.equals("") || password1.equals("") || password2.equals("") ){
				legal = false;
				message += "Error: All fields are required<br />";
			}
			
			if(legal){
				String query2 = "INSERT INTO testtable (username, password) VALUES ('"+username+"', '"+password1+"')";
				s = con.createStatement();
				s.executeUpdate(query2);
				message += "User added successfully!<br />";
			}
			session.setAttribute("reg_message", message);
			response.sendRedirect("register.jsp");
			
	}
	catch(Exception e){
		out.println("Exception: " + e.getMessage() + "<br />");
	}
%>	
	
