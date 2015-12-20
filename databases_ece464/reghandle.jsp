<%@ page import="java.sql.*" %>
<%
Connection con = (Connection)session.getAttribute("con");
	if(con == null){
		response.sendRedirect("madawesomehtmllogin.html");
		return;
	}
	/*ResultSet rs = (ResultSet)session.getAttribute("rs");
	if(rs == null){
		response.sendRedirect("madawesomehtmllogin.html");
		return;
	}*/

	String username = request.getParameter("username");
	String password1 = request.getParameter("password1");
	String password2 = request.getParameter("password2");
	String name = request.getParameter("name");
	String email = request.getParameter("email");
	String lname = request.getParameter("lname");
	String gname = request.getParameter("gname");

	session.setAttribute("reg_username", username);
	session.setAttribute("reg_name", name);
	session.setAttribute("reg_email", email);
	session.setAttribute("reg_lname", lname);
	session.setAttribute("reg_gname", gname);
	
	
	try{
		Class.forName("com.mysql.jdbc.Driver").newInstance();
		con = DriverManager.getConnection("jdbc:mysql://localhost/OfficeDB", "default", "madoffice");
		if(!con.isClosed())
			out.println("Successfully connected to MySQL server.<br />");
		Statement s = con.createStatement();
			boolean legal = true;
			String message = "";
			
			String query1 = "SELECT * FROM Employees WHERE username='"+username+"'";
			String query3 = "SELECT * FROM Groups WHERE gname='"+gname+"'";
			String query4 = "SELECT * FROM Locations WHERE lname='"+lname+"'";
			s.executeQuery(query1);
			ResultSet rs2 = s.getResultSet();
			if(rs2.next()){
				legal = false;
				message += "Error: Username already exists<br />";
			}
			/*if(rs2 != null){
				legal = false;
			}*/
			if(password1.equals(password2) == false){
				legal = false;
				message += "Error: Passwords do not match<br />";
			}
			if(username.equals("") || password1.equals("") || password2.equals("") || email.equals("") || lname.equals("") || gname.equals("")){
				legal = false;
				message += "Error: All fields are required<br />";
			}
			s.executeQuery(query3);
			ResultSet rs3 = s.getResultSet();
			if(!rs3.next()){
				legal = false;
				message += "Error: Group does not exist<br />";
			}
			/*if(rs3 == null){
				legal = false;
			}*/
			s.executeQuery(query4);
			ResultSet rs4 = s.getResultSet();
			if(!rs4.next()){
				legal = false;
				message += "Error: Location does not exist<br />";
			}
			/*if(rs4 == null){
				legal = false;
			}*/
			
			if(legal){
				String query2 = "INSERT INTO Employees (username, password, ename, email, lname, gname) VALUES ('"+username+"', '"+password1+"', '"+name+"', '"+email+"', '"+lname+"', '"+gname+"')";
				s = con.createStatement();
				s.executeUpdate(query2);
				message += "Employee added successfully!<br />";
			}
			session.setAttribute("reg_message", message);
			response.sendRedirect("register.jsp");
			
	}
	catch(Exception e){
		out.println("Exception: " + e.getMessage() + "<br />");
	}
%>	
	