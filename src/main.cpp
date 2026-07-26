#include "Application.h"

int main() {
  Core::ApplicationSpecification appSpec;
  appSpec.Name = "Sentinel";
  appSpec.WindowSpec.Width = 1920;
  appSpec.WindowSpec.Height = 1080;

  Core::Application application(appSpec);
  application.Run();
}