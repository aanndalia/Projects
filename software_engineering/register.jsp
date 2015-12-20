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
			</td>
			<td>
				<input type="text" name="username" maxlength=20 size=30 <%= session.getAttribute("reg_username") == null ? "" : "value=\""+(String)session.getAttribute("reg_username")+"\"" %>/> <br />
				<input type="password" name = "password1" maxlength=20 size=30/> <br />
				<input type="password" name = "password2" maxlength=20 size=30/> <br />
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
	session.setAttribute("reg_message", null);
%>