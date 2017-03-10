<?php 
	// 作者：Gavin 林智杰
	// 团队：贝利珠海贼团
	// 项目: 鲸鱼刷卡机
	// 学校：北京理工大学珠海学院
	// 邮箱：543541941@qq.com
	//绑定业务操作
	$userid=$_POST['userid'];
	$cardid=$_POST['username'];
	$pass=$_POST['password'];
	$conn=mysql_connect("localhost","root","admin");
	mysql_select_db("US");
	mysql_query("set names utf8");
	$sql="select * from us where cardid='{$cardid}' and pass='{$pass}'";
	$res=mysql_query($sql);
	//判断是否有存在该刷卡机
	if ($row=mysql_fetch_assoc($res)) {
		$sql="update us set userid='{$userid}'";
		//判断是否绑定成功
		if(mysql_query($sql))
		{
			echo "<script>alert('绑定成功！')</script>";			
		
		}else{
			echo "<script>alert('绑定失败！')</script>";
			echo "<script>location='bangding.php?$id='{$userid}''</script>";
		}
	}else{
		echo "<script>alert('用户不存在，绑定失败！')</script>";
		echo "<script>location='bangding.php?$id='{$userid}''</script>";
	}
	mysql_close($conn);
 ?>