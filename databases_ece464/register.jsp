<html>
	<body>
	<%= session.getAttribute("reg_message") == null ? "" : (String)session.getAttribute("reg_message") %>
	<form action="reghandle.jsp" method="post">
	<table>
		<tr>
			<td>
				Username: <br />
				Password: <br />
				Confirm Password: <br />
				Name: <br />
				Email: <br />
				Location: <br />
				Group: <br />
			</td>
			<td>
				<input type="text" name="username" maxlength=20 size=30 <%= session.getAttribute("reg_username") == null ? "" : "value=\""+(String)session.getAttribute("reg_username")+"\"" %>/> <br />
				<input type="password" name = "password1" maxlength=20 size=30/> <br />
				<input type="password" name = "password2" maxlength=20 size=30/> <br />
				<input type="text" name = "name" maxlength=30 size=30 <%= session.getAttribute("reg_name") == null ? "" : "value=\""+(String)session.getAttribute("reg_name")+"\"" %>/><br />
				<input type="text" name = "email" maxlength=30 size=30 <%= session.getAttribute("reg_email") == null ? "" : "value=\""+(String)session.getAttribute("reg_email")+"\"" %>/><br />
				<input type="text" name = "lname" maxlength=15 size=30 <%= session.getAttribute("reg_lname") == null ? "" : "value=\""+(String)session.getAttribute("reg_lname")+"\"" %>/><br />
				<input type="text" name = "gname" maxlength=15 size=30 <%= session.getAttribute("reg_gname") == null ? "" : "value=\""+(String)session.getAttribute("reg_gname")+"\"" %>/><br />
			</td>
		</tr>
		<tr>
			<td>
			</td>
			<td align="right">
				<input type="submit" name="button" value = "Register!">
			</td>
		</tr>
	</table>
	</form>
	</body>
</html>
<%
	session.setAttribute("reg_username", null);
	session.setAttribute("reg_name", null);
	session.setAttribute("reg_email", null);
	session.setAttribute("reg_lname", null);
	session.setAttribute("reg_gname", null);
	session.setAttribute("reg_message", null);
%>