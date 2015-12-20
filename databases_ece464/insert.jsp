<%@ page import="java.sql.*" %>
<%@ page import="java.text.SimpleDateFormat" %>
<%@ page import="java.util.Date" %>
<%

/*
	for(int i = 0; ; i++){
		String status = (String)request.getParameter("Status"+i);
		if(status == null){
			out.println("Cannot find " +"Status"+i+ "<br />");
			break;
		}
		out.println("Status"+i+": "+status+"<br />");
	}
*/

	Connection con = (Connection)session.getAttribute("con");
	if(con == null){
		response.sendRedirect("madawesomehtmllogin.html");
		return;
	}
	ResultSet rs = (ResultSet)session.getAttribute("rs");
	if(rs == null){
		response.sendRedirect("madawesomehtmllogin.html");
		return;
	}
	Date dateEntered = (Date)session.getAttribute("dateEntered");
	if(dateEntered == null){
		response.sendRedirect("madawesomehtmllogin.html");
		return;
	}
	SimpleDateFormat dbFormat = new SimpleDateFormat("yyyy-MM-dd");

	String button = (String)request.getParameter("button");
	if(button.equals("Update")){
		int count = 0;
		
		for(int j = 0; j < 12; j++){
			String[] snames = {"Morn", "Aftn", "Nite"};
			String sname = snames[j%3];
			Date temp = new Date();
			temp.setTime(dateEntered.getTime() + (j/3)*24*60*60*1000);
			String str2 = "SELECT * FROM Shifts WHERE sname = '"+sname+"' AND sdate = '"+dbFormat.format(temp)+"'";
			Statement s2 = con.createStatement();
			s2.executeQuery(str2);
			ResultSet rs2 = s2.getResultSet();
			if(!rs2.next()){
				str2 = "INSERT INTO Shifts (sname, sdate) VALUES ('"+sname+"', '"+dbFormat.format(temp)+"')";
				s2 = con.createStatement();
				s2.executeUpdate(str2);
			}

			rs.beforeFirst();
			while(rs.next()){
				String status = (String)request.getParameter("Status"+count);
				if(status == null){
					out.println("<br />Cannot find " +"Status"+count+ "<br />");
					return;
				}
				
				str2 = "SELECT * FROM Works WHERE username = '"+rs.getString("username")+"' AND sname = '"+sname+"' AND sdate = '"+dbFormat.format(temp)+"'";
				s2 = con.createStatement();
				s2.executeQuery(str2);
				rs2 = s2.getResultSet();
				if(!rs2.next()){
					if(status.equals("----"))
						str2 = "INSERT INTO Works (username, sname, sdate) VALUES ('"+rs.getString("username")+"', '"+sname+"', '"+dbFormat.format(temp)+"')";
					else
						str2 = "INSERT INTO Works (username, sname, sdate, status) VALUES ('"+rs.getString("username")+"', '"+sname+"', '"+dbFormat.format(temp)+"', '"+status+"')";
				}
				else{
					str2 = "UPDATE Works SET status="+(status.equals("----")?"NULL":"'"+status+"'")+" WHERE username = '"+rs.getString("username")+"' AND sname = '"+sname+"' AND sdate = '"+dbFormat.format(temp)+"'";
				}
				s2 = con.createStatement();
				s2.executeUpdate(str2);
				count += 12;
			}
			count = j+1;
		}
	}
	else if(button.equals("Previous")){
		dateEntered.setTime(dateEntered.getTime() - 4*24*60*60*1000);
		session.setAttribute("dateEntered", dateEntered);
	}
	else if(button.equals("Next")){
		dateEntered.setTime(dateEntered.getTime() + 4*24*60*60*1000);
		session.setAttribute("dateEntered", dateEntered);
	}
	Boolean showTable = new Boolean(true);
	session.setAttribute("showTable", showTable);
	response.sendRedirect("manager.jsp");
%>
