<html>
	<body>
	<%= session.getAttribute("unreg_message") == null ? "" : (String)session.getAttribute("unreg_message") %>
	<form action="unreghandle.jsp" method="post">
	<table>
		<tr>
			<td>
			Enter username for employee to delete: <br />
			</td>
			<td>
			<input type="text" name = "del_username" maxlength=20 size=20 <%= session.getAttribute("unreg_username") == null ? "" : "value=\""+(String)session.getAttribute("unreg_username")+"\"" %>/> <input type="submit" name="button" value = "Unregister">
			</td>
		</tr>
	</table>
	</form>
	</body>
</html>
<%
	session.setAttribute("unreg_username", null);
	session.setAttribute("unreg_message", null);
%>