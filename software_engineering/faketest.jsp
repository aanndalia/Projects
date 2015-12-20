<%@ page import="java.sql.*" %>
<%@ page import="java.text.SimpleDateFormat" %>
<%@ page import="java.util.Date" %>

<% 	
	Connection con = (Connection)session.getAttribute("con");
	if(con == null){
		response.sendRedirect("setlogin.html");
		return;
	}
%>
<html>
	<body>
		<div style="position:absolute;top:5;right:5;"><form action="logout.jsp" method="post">
			<input type="submit" value="Logout" name="button" />
		</form></div>
		<form action="faketest.jsp" method="post">
		This is a fake test.
		<p> This page should only be reachable for those who have logged in </p>
		</form>
	</body>
</html>
