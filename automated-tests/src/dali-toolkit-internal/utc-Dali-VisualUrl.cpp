/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>

#include <stdlib.h>

#include <dali/devel-api/common/hash.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

using namespace Dali::Toolkit::Internal;

int UtcDaliVisualUrlConstructor(void)
{
  const char* url = "file://bar.org/foobar.gif";
  VisualUrl   visualUrl(url);
  DALI_TEST_EQUALS(true, visualUrl.IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl.GetType(), VisualUrl::GIF, TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl.GetProtocolType(), VisualUrl::LOCAL, TEST_LOCATION);

  VisualUrl visualUrl2("foobar.jpeg");
  visualUrl2 = visualUrl;
  DALI_TEST_EQUALS(true, visualUrl2.IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl2.GetType(), VisualUrl::GIF, TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl2.GetProtocolType(), VisualUrl::LOCAL, TEST_LOCATION);

  VisualUrl visualUrl3(visualUrl);
  DALI_TEST_EQUALS(true, visualUrl3.IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl3.GetType(), VisualUrl::GIF, TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl3.GetProtocolType(), VisualUrl::LOCAL, TEST_LOCATION);

  VisualUrl visualUrl4("dali://0");
  visualUrl4 = visualUrl;
  DALI_TEST_EQUALS(true, visualUrl4.IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl4.GetType(), VisualUrl::GIF, TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl4.GetProtocolType(), VisualUrl::LOCAL, TEST_LOCATION);

  VisualUrl visualUrl5("dali://1");
  visualUrl4 = visualUrl5;
  DALI_TEST_EQUALS(true, visualUrl4.IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl4.GetType(), VisualUrl::REGULAR_IMAGE, TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl4.GetProtocolType(), VisualUrl::TEXTURE, TEST_LOCATION);

  VisualUrl visualUrl6("enbuf://0");
  visualUrl6 = visualUrl;
  DALI_TEST_EQUALS(true, visualUrl6.IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl6.GetType(), VisualUrl::GIF, TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl6.GetProtocolType(), VisualUrl::LOCAL, TEST_LOCATION);

  VisualUrl visualUrl7("enbuf://1");
  visualUrl6 = visualUrl7;
  DALI_TEST_EQUALS(true, visualUrl6.IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl6.GetType(), VisualUrl::REGULAR_IMAGE, TEST_LOCATION);
  DALI_TEST_EQUALS(visualUrl6.GetProtocolType(), VisualUrl::BUFFER, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVisualUrlRegularImage(void)
{
  tet_infoline("UtcDaliVisualUrl REGULAR_IMAGE");

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("foobar.jpeg").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("foobar.PNG").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("foobar.Png123").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("foobar.Png1.23").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl(" ").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl(".").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("9").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("dali://bar.org/foobar.gif").GetType(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUrlSvg(void)
{
  tet_infoline("UtcDaliVisualUrl SVG");

  DALI_TEST_EQUALS(VisualUrl::SVG, VisualUrl("foobar.svg").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::SVG, VisualUrl("foobar.svg.svg").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::SVG, VisualUrl("foobar.svG").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::SVG, VisualUrl("foobar.SVG").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::SVG, VisualUrl(".SvG").GetType(), TEST_LOCATION);

  // SVGs aren't N-patch
  DALI_TEST_EQUALS(VisualUrl::SVG, VisualUrl("foobar.9.svg").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("svg.png").GetType(), TEST_LOCATION);

  // maybe controversial, but for now we expect the suffix to be exactly .svg
  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("svg.svg1").GetType(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUrlNPatch(void)
{
  tet_infoline("UtcDaliVisualUrl N_PATCH");

  DALI_TEST_EQUALS(VisualUrl::N_PATCH, VisualUrl("foobar.#.png").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::N_PATCH, VisualUrl("foobar.9.9.bmp").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::N_PATCH, VisualUrl("foobar.9.9.jpg[]=$$").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::N_PATCH, VisualUrl("foobar.9.#.#.9.wbpm123").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("svg.##.png").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("svg.99.jpeg").GetType(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUrlGif(void)
{
  tet_infoline("UtcDaliVisualUrl GIF");

  DALI_TEST_EQUALS(VisualUrl::GIF, VisualUrl("foobar.gif").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::GIF, VisualUrl("foobar.gif.gif").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::GIF, VisualUrl("foobar.giF").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::GIF, VisualUrl("foobar.GIF").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::GIF, VisualUrl(".GiF").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::N_PATCH, VisualUrl("foobar.9.gif").GetType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("gif.png").GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("gif.gif1").GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REGULAR_IMAGE, VisualUrl("dali://.gif").GetType(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUrlLocationP(void)
{
  tet_infoline("UtcDaliVisualUrl Location");

  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("foobar.GIF").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("file://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("file://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("file://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("file://bar.org/foobar.jpeg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("file://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ftp://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ftp://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ftp://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ftp://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ftp://bar.org/foobar.GIF").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ftp://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ssh://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ssh://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ssh://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ssh://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ssh://bar.org/foobar.GIF").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("ssh://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("http://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("http://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("http://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("http://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("http://bar.org/foobar.GIF").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("http://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("https://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("https://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("https://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("https://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("https://bar.org/foobar.GIF").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("https://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("FTP://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("FTP://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("FTP://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("FTP://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("FTP://bar.org/foobar.GIF").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("FTP://BAR.ORG/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("SSH://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("SSH://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("SSH://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("SSH://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("SSH://bar.org/foobar.GIF").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("SSH://BAR.ORG/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTP://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTP://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTP://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTP://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTP://bar.org/foobar.GIF").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTP://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTPS://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTPS://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTPS://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTPS://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTPS://bar.org/foobar.GIF").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::REMOTE, VisualUrl("HTTPS://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::TEXTURE, VisualUrl("dali://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::TEXTURE, VisualUrl("dali://1234").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::TEXTURE, VisualUrl("DALI://1234").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::TEXTURE, VisualUrl("dali://.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::TEXTURE, VisualUrl("dali://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::TEXTURE, VisualUrl("dali://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::TEXTURE, VisualUrl("dali://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::TEXTURE, VisualUrl("dali://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::BUFFER, VisualUrl("enbuf://").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::BUFFER, VisualUrl("enbuf://1234").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::BUFFER, VisualUrl("ENBUF://1234").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::BUFFER, VisualUrl("enbuf://.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::BUFFER, VisualUrl("enbuf://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::BUFFER, VisualUrl("enbuf://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::BUFFER, VisualUrl("enbuf://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::BUFFER, VisualUrl("enbuf://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUrlLocationN(void)
{
  tet_infoline("UtcDaliVisualUrl Location negative tests");

  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("h://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("ht://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("htp://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("htpp://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("httt://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("http;//bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("http:x/bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("http:/xbar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("sshttp://bar.org/foobar.svg").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("http:https://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("https:http://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("HPPT://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("ftp:/bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("ftp:a/bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("fpp://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("ftt://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("ssh;//bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("ssh:/bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("ssh:a/bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("shh://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("sss://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("fsh://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("stp://bar.org/foobar.9.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("http:/bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("h1tps://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("ht2ps://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("htt3s://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("http4://bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("https5/bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("https:6/bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("https:/7bar.org/foobar.gif").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("file://bar.org/foobar.png").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("dal://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("d1li://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("da2i://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("dal3://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("dali4//1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("dali:5/1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("dali:/61").GetProtocolType(), TEST_LOCATION);

  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("eunki://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("enbu://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("eubnf://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("1nbuf://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("e2bun://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("en3uf://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("enb4f://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("enbu5://1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("enbuf6//1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("enbuf:7/1").GetProtocolType(), TEST_LOCATION);
  DALI_TEST_EQUALS(VisualUrl::LOCAL, VisualUrl("enbuf:/81").GetProtocolType(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUrlIsValid(void)
{
  tet_infoline("UtcDaliVisualUrl IsValid");

  DALI_TEST_EQUALS(false, VisualUrl().IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("").IsValid(), TEST_LOCATION);

  DALI_TEST_EQUALS(true, VisualUrl("foobar.gif").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("foobar.png").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("foobar.svg").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("foobar.GIF").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("foobar.9.png").IsValid(), TEST_LOCATION);

  DALI_TEST_EQUALS(true, VisualUrl("http://bar.org/foobar.gif").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("http://bar.org/foobar.png").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("http://bar.org/foobar.svg").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("http://bar.org/foobar.GIF").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("http://bar.org/foobar.9.png").IsValid(), TEST_LOCATION);

  DALI_TEST_EQUALS(true, VisualUrl("https://bar.org/foobar.gif").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("https://bar.org/foobar.png").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("https://bar.org/foobar.svg").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("https://bar.org/foobar.GIF").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("https://bar.org/foobar.9.png").IsValid(), TEST_LOCATION);

  DALI_TEST_EQUALS(true, VisualUrl("HTTP://bar.org/foobar.gif").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("HTTP://bar.org/foobar.png").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("HTTP://bar.org/foobar.svg").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("HTTP://bar.org/foobar.GIF").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("HTTP://bar.org/foobar.9.png").IsValid(), TEST_LOCATION);

  DALI_TEST_EQUALS(true, VisualUrl("HTTPS://bar.org/foobar.gif").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("HTTPS://bar.org/foobar.png").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("HTTPS://bar.org/foobar.svg").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("HTTPS://bar.org/foobar.GIF").IsValid(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("HTTPS://bar.org/foobar.9.png").IsValid(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUrlIsLocalResource(void)
{
  tet_infoline("UtcDaliVisualUrl IsLocalResource");

  DALI_TEST_EQUALS(true, VisualUrl("foobar.gif").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("foobar.png").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("foobar.svg").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("foobar.GIF").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, VisualUrl("foobar.9.png").IsLocalResource(), TEST_LOCATION);

  DALI_TEST_EQUALS(false, VisualUrl("http://bar.org/foobar.gif").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("http://bar.org/foobar.png").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("http://bar.org/foobar.svg").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("http://bar.org/foobar.GIF").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("http://bar.org/foobar.9.png").IsLocalResource(), TEST_LOCATION);

  DALI_TEST_EQUALS(false, VisualUrl("https://bar.org/foobar.gif").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("https://bar.org/foobar.png").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("https://bar.org/foobar.svg").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("https://bar.org/foobar.GIF").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("https://bar.org/foobar.9.png").IsLocalResource(), TEST_LOCATION);

  DALI_TEST_EQUALS(false, VisualUrl("HTTP://bar.org/foobar.gif").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("HTTP://bar.org/foobar.png").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("HTTP://bar.org/foobar.svg").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("HTTP://bar.org/foobar.GIF").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("HTTP://bar.org/foobar.9.png").IsLocalResource(), TEST_LOCATION);

  DALI_TEST_EQUALS(false, VisualUrl("HTTPS://bar.org/foobar.gif").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("HTTPS://bar.org/foobar.png").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("HTTPS://bar.org/foobar.svg").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("HTTPS://bar.org/foobar.GIF").IsLocalResource(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, VisualUrl("HTTPS://bar.org/foobar.9.png").IsLocalResource(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualUrlGetLocationP(void)
{
  tet_infoline("UtcDaliVisualUrl GetLocation Positive");

  DALI_TEST_EQUAL("a", VisualUrl("http://a").GetLocation());
  DALI_TEST_EQUAL("1", VisualUrl("dali://1").GetLocation());
  DALI_TEST_EQUAL("4", VisualUrl("enbuf://4").GetLocation());
  DALI_TEST_EQUAL("", VisualUrl("ftp://").GetLocation());
  DALI_TEST_EQUAL("http://", VisualUrl("http://http://").GetLocation());

  END_TEST;
}

int UtcDaliVisualUrlGetLocationN(void)
{
  tet_infoline("UtcDaliVisualUrl GetLocation Negative");

  DALI_TEST_EQUAL("", VisualUrl("").GetLocation());
  DALI_TEST_EQUAL("a", VisualUrl("a").GetLocation());
  DALI_TEST_EQUAL("dali:/1", VisualUrl("dali:/1").GetLocation());
  DALI_TEST_EQUAL("dali//1", VisualUrl("dali//1").GetLocation());
  DALI_TEST_EQUAL("enbuf:/2", VisualUrl("enbuf:/2").GetLocation());
  DALI_TEST_EQUAL("", VisualUrl("http:/http://").GetLocation());

  END_TEST;
}

int UtcDaliVisualUrlCreateTextureUrl(void)
{
  tet_infoline("UtcDaliVisualUrl CreateTextureUrl");

  DALI_TEST_EQUAL("dali://a", VisualUrl::CreateTextureUrl("a"));
  DALI_TEST_EQUAL("dali://1234", VisualUrl::CreateTextureUrl("1234"));
  DALI_TEST_EQUAL("dali://", VisualUrl::CreateTextureUrl(""));

  END_TEST;
}

int UtcDaliVisualUrlCreateBufferUrl(void)
{
  tet_infoline("UtcDaliVisualUrl CreateBufferUrl");

  DALI_TEST_EQUAL("enbuf://a", VisualUrl::CreateBufferUrl("a"));
  DALI_TEST_EQUAL("enbuf://1234", VisualUrl::CreateBufferUrl("1234"));
  DALI_TEST_EQUAL("enbuf://", VisualUrl::CreateBufferUrl(""));

  END_TEST;
}

int UtcDaliVisualUrlGetUrlHash(void)
{
  tet_infoline("UtcDaliVisualUrl GetUrlHash");

  std::vector<std::string> testUrlLists =
    {
      "", // empty string.
      "/opt/usr/home/owner/media/resources/test.png",
      "hello.world",
      "image.jpg",
      "not_suffix",
      "https://bar.org/foo/bar.gif",
      "dali://0",
      "enbuf://1",
    };

  for(auto& url : testUrlLists)
  {
    VisualUrl visualUrl(url);

    std::uint64_t exceptHash = Dali::CalculateHash(url);
    std::uint64_t visualHash = visualUrl.GetUrlHash();

    DALI_TEST_EQUAL(visualHash, exceptHash);
  }

  END_TEST;
}