<%@ page import="java.sql.*" %>
<%
Connection con = (Connection)session.getAttribute("con");
	if(con == null){
		response.sendRedirect("madawesomehtmllogin.html");
		return;
	}
	
	String del_username = request.getParameter("del_username");
	session.setAttribute("unreg_username", del_username);
	
	String query1 = "SELECT * FROM Employees WHERE username='"+del_username+"'";
	Statement s = con.createStatement();
	s.executeQuery(query1);
	ResultSet rs2 = s.getResultSet();
	String message = "";
	if(!rs2.next()){
		message += "Error: Username does not exist <br />";
	}
	else{
		String query2 = "DELETE FROM Employees WHERE username='"+del_username+"'";
		s.executeUpdate(query2);
		message += "Employee deleted successfully <br /> ";
	}
	session.setAttribute("unreg_message", message);
	response.sendRedirect("unregister.jsp");
%>