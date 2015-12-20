<%@ page import="java.sql.*" %>
<%@ page import="java.text.SimpleDateFormat" %>
<%@ page import="java.util.Date" %>
<%!
	boolean in_array(String needle, String[] haystack){
		for(String s : haystack)
			if(s.equals(needle))
				return true;
		return false;
	}
%>
<% 	
	Connection con = (Connection)session.getAttribute("con");
	if(con == null){
		response.sendRedirect("madawesomehtmllogin.html");
		return;
	}
	if(session.getAttribute("gname").equals("Manager")){
		//manager is attempting to access regular page
		response.sendRedirect("madawesomehtmllogin.html");
		return;
	}
%>
<html>
	<head>
		<title> Schedule Viewer </title>
		<style type="text/css">
			.left{
				border-left: 2px solid black;
			}
			.right{
				border-right: 2px solid black;
			}
			.top{
				border-top: 2px solid black;
			}
			.bottom{
				border-bottom: 2px solid black;
			}
		</style>
	</head>
	<body>
		<div style="position:absolute;top:5;right:5;"><form action="logout.jsp" method="post">
			<input type="submit" value="Logout" name="button" />
		</form></div>
		<form action="regular.jsp" method="post">
			<select name="Groups" multiple="yes">
				<%
					Statement s = con.createStatement();
					String str = "SELECT gname FROM Groups";
					s.executeQuery(str);
					ResultSet rs = s.getResultSet();
					while(rs.next()){
						String gname = rs.getString("gname");
						if(request.getParameterValues("Groups") != null && in_array(gname, (String[])request.getParameterValues("Groups")))
							out.println("<option value=\""+gname+"\" selected=\"selected\"> "+ gname +" </option>");
						else
							out.println("<option value=\""+gname+"\"> "+ gname +" </option>");
					}
				%>
			</select>
			<br />
			<br />
			<div>
				<select name = "Location">
					<%
						str = "SELECT lname FROM Locations";
						s.executeQuery(str);
						rs = s.getResultSet();
						while(rs.next()){
							String lname = rs.getString("lname");
							if(request.getParameter("Location") != null && lname.equals((String)request.getParameter("Location")))
								out.println("<option value=\"" +lname+ "\" selected=\"selected\"> " + lname + " </option>");
							else
								out.println("<option value=\"" +lname+ "\"> " + lname + " </option>");
						}
					%>
				</select>
				Date <input type="text" size = "2" maxlength="2" name="month" value="<%= request.getParameter("month") == null ? "mm" : (String)request.getParameter("month") %>" />
				<input type="text" size = "2" maxlength="2" name="day" value="<%= request.getParameter("day") == null ? "dd" : (String)request.getParameter("day") %>" />
				<input type="text" size = "4" maxlength="4" name="year" value="<%= request.getParameter("year") == null ? "yyyy" : (String)request.getParameter("year") %>" />
				<input type="submit" value="Go!" name="button" />
			</div>
			<%
				String button = (String)request.getParameter("button");
				if(button != null) {
				//validate input
				SimpleDateFormat dbFormat = new SimpleDateFormat("yyyy-MM-dd");
				SimpleDateFormat displayFormat = new SimpleDateFormat("EEE MM/dd/yy");
				Date dateEntered;
				String[] groups;
				String location;
				if(button.equals("Go!")){
					try{
						dateEntered = dbFormat.parse((String)request.getParameter("year")+"-"+(String)request.getParameter("month")+"-"+(String)request.getParameter("day"));
					}
					catch(java.text.ParseException e){
						out.println("<p><b>Error</b>: You entered an invalid date</p>");
						out.println("</body>");
						out.println("</html>");
						return;
					}
					groups = request.getParameterValues("Groups");
					location = request.getParameter("Location");
					if(groups == null){
						out.println("<p><b>Error</b>: You must select at least one group</p>");
						out.println("</body>");
						out.println("</html>");
						return;
					}
					session.setAttribute("groups", groups);
					session.setAttribute("location", location);
				}
				else{
					dateEntered = (Date)session.getAttribute("dateEntered");
					if(button.equals("Previous"))
						dateEntered.setTime(dateEntered.getTime() - 4*24*60*60*1000);
					else
						dateEntered.setTime(dateEntered.getTime() + 4*24*60*60*1000);
					groups = (String[])session.getAttribute("groups");
					location = (String)session.getAttribute("location");
				}
				session.setAttribute("dateEntered", dateEntered);
			%>
			<br />
			<table cellspacing="0">
				<tr>
					<td class="right">&nbsp;</td>
			<%
				for(int i=0;i<4;i++){
					Date temp = new Date();
					temp.setTime(dateEntered.getTime() + i*24*60*60*1000);
					out.println("<td colspan=3 class=\"right\" style=\"text-align:center;\">" + displayFormat.format(temp) +  "</td>");
				}
			%>
				</tr>
				<tr>
					<td class="right bottom">&nbsp;</td>
					<%
						for(int q=0;q<4;q++){
							for(int p=0;p<3;p++){
								if(p == 0){
									out.println("<td class=\"right bottom\"> Morn </td>");
								}
								else if(p == 1){
									out.println("<td class=\"right bottom\"> Aftn </td>");
								}
								else if(p == 2){
									out.println("<td class=\"right bottom\"> Nite </td>");
								}
							}
						}
					%>
				</tr>
					<%
						str = "SELECT gname,ename,username FROM Employees WHERE lname='"+location+"' AND (";
						for(int i = 0; i < groups.length; i++){
							str += "gname='"+groups[i]+"'";
							if(i < groups.length - 1){
								str += " OR ";
							}
						}
						str += ") ORDER BY gname,ename";
						s.executeQuery(str);
						rs = s.getResultSet();
						while(rs.next()){
							out.println("<tr>");
							out.println("<td class=\"right bottom\"> " + rs.getString("ename") + " (" + rs.getString("gname") + ")" + " </td>");
							for(int j = 0; j < 12; j++){
								Date temp = new Date();
								String[] snames = {"Morn", "Aftn", "Nite"};
								temp.setTime(dateEntered.getTime() + (j/3)*24*60*60*1000);
								String str2 = "SELECT status FROM Works w, Employees e WHERE w.username = e.username AND e.username = '" + rs.getString("username") + "' AND sdate = '" + dbFormat.format(temp) + "' AND sname = '" + snames[j%3] + "'";
								Statement s2 = con.createStatement();
								s2.executeQuery(str2);
								ResultSet rs2 = s2.getResultSet();
								if(!rs2.next() || rs2.getString("status") == null)
									out.println("<td class=\"right bottom\"> ---- </td>");
								else
									out.println("<td class=\"right bottom\"> " + rs2.getString("status") + " </td>");
							}
							out.println("</tr>");
						}
					%>
			</table>
			<br />
			<input type="submit" name="button" value="Previous" />
			<input type="submit" name="button" value="Next" />
			<%
				}
			%>
		</form>
	</body>
</html>
