<!DOCTYPE html>
<html lang="en" class="no-js">
    <head>
        <meta charset="utf-8">
        <title>鲸鱼刷卡机</title>
        <meta content="initial-scale=1.0, minimum-scale=1.0, maximum-scale=2.0, user-scalable=no, width=device-width" name="viewport">
        
        <!-- CSS -->
        <link rel="stylesheet" href="assets/css/reset.css">
        <link rel="stylesheet" href="assets/css/supersized.css">
        <link rel="stylesheet" href="assets/css/style.css">

    </head>

    <body>

        <div class="page-container">
            <h1>用户绑定</h1>
            <form action="update.php" method="post">
                <input type="text" name="username" class="username" placeholder="学号">
                <input type="password" name="password" class="password" placeholder="消费密码">
                <input type="hidden" name="userid" value="<?php echo $_GET['id'] ?>">
                <button type="submit">提交</button>
                <div class="error"><span>+</span></div>
            </form>
        </div>
		
        <!-- Javascript -->
        <script src="assets/js/jquery-1.8.2.min.js"></script>
        <script src="assets/js/supersized.3.2.7.min.js"></script>
        <script src="assets/js/supersized-init.js"></script>
        <script src="assets/js/scripts.js"></script>

    </body>

</html>


